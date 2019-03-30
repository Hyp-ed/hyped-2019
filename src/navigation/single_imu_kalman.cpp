/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 10/03/2019
 * Description: single IMU measurement filtered with Kalman Filter and written to file
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "navigation/single_imu_kalman.hpp"

namespace hyped
{
    namespace navigation
    {
        SingleImuKalmanNavigation::SingleImuKalmanNavigation(ImuQuery& imuQuery_, int imuId_,
                                                             GravityCalibration& gravityCalibrator_,
                                                             Timer* timer_,
                                                             unsigned int n_, unsigned int m_)
            : imuQuery(imuQuery_),
              imuId(imuId_),
              gravityCalibrator(gravityCalibrator_),
              timer(timer_),
              kalmanManager(KalmanManager(n_, m_))
        {}

        SingleImuKalmanNavigation::SingleImuKalmanNavigation(ImuQuery& imuQuery_, int imuId_,
                                                             GravityCalibration& gravityCalibrator_,
                                                             Timer* timer_, unsigned int n_
                                                             unsigned int m_, unsigned int k_)
            : imuQuery(imuQuery_),
              imuId(imuId_),
              gravityCalibrator(gravityCalibrator_),
              timer(timer_),
              kalmanManager(KalmanManager(n_, m_, k_))
        {}

        int SingleImuKalmanNavigation::navigate(unsigned int nTestQueries, float queryDelay, int runId, Logger log)
        {
            // File setup
            bool writeToFile = (sys.imu_id > 0) || (sys.run_id > 0);
            std::ofstream outfile;

            // IMU data logger
            ImuDataLogger imuDataLogger(outfile);
            imuDataLogger.setup(imuId, runId);

            // setup IMU manager
            double current_time = -1.0;
            std::string filterSetup = "stationary";
            if (filterSetup == "elevator")
            {
                kalmanManager.setupElevator();
            } else if (filterSetup == "stationary")
            {
                kalmanManager.setupStationary();
            } else
            {
                throw "Unknown filterSetup!";
            }

            // Calibrate gravitational acceleration
            NavigationVector gVector = gravityCalibrator.calibrate(imuQuery, timer);
            log.INFO("SINGLE_IMU_KALMAN", "Calibration complete, measuring.");

            // Return measured gravity vector
            log.INFO("SINGLE_IMU_KALMAN", "Measured gravity vector:\n\tgx=%+6.3f\tgy=%+6.3f\tgz=%+6.3f\n\n",
                                           gVector[0], gVector[1], gVector[2]);

            // Store measured/estimated values
            DataPoint<NavigationVector> accRaw(0., NavigationVector({0.,0.,0.}));
            DataPoint<NavigationVector> accCor(0., NavigationVector({0.,0.,0.})); 	
            DataPoint<NavigationVector>    vel(0., NavigationVector({0.,0.,0.}));		
            DataPoint<NavigationVector>    pos(0., NavigationVector({0.,0.,0.}));
            //
            // Integrate acceleration -> velocity -> position
            Integrator<NavigationVector> velIntegrator(&vel);
            Integrator<NavigationVector> posIntegrator(&pos);

            // Perform acceleration, speed and distance measurements
            for (unsigned int i = 0; i < nTestQueries; ++i)
            {
                // Query sensor and correct values
                accRaw = imuQuery.query();
                accCor = DataPoint<NavigationVector>(accRaw.timestamp, accRaw.value - gVector);

                if (current_time == -1.0)
                {
                    kalmanManager.updateStateTransitionMatrix(0.0);
                } else
                {
                    // compute passed time dt
                    double dt = (accRaw.timestamp - current_time)/1000000.0
                    kalmanManager.updateStateTransitionMatrix(dt);
                }
                current_time = accRaw.timestamp;

                kalmanManager.filter(accCor.value);

                // Intergrate
                velIntegrator.update(accCor);
                posIntegrator.update(vel);

                // Output values
                log.INFO("MAIN", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
                                accCor.value[0], accCor.value[1], accCor.value[2], 
                                vel.value[0], vel.value[1], vel.value[2], 
                                pos.value[0], pos.value[1], pos.value[2]);		

                NavigationEstimate x = kalmanManager.getEstimate();
                log.INFO("FILTERED", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
                                x[2][0], x[2][1], x[2][2],
                                x[1][0], x[1][1], x[1][2],
                                x[0][0], x[0][1], x[0][2];


                if (writeToFile > 0) imuDataLogger.dataToFile(&accRaw, &accCor, &vel, &pos);
                sleep(queryDelay);
            }

            if (writeToFile) outfile.close();
            return 0;
        }
    }
}
