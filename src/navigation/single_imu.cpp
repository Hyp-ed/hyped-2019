/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 02/02/2019
 * Description: Simple single IMU measurement written to file
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

#include "single_imu.hpp"

namespace hyped
{
    namespace navigation
    {

        SingleImuNavigation::SingleImuNavigation(ImuQuery& imuQuery_, int imuId_,
                                                 GravityCalibration& gravityCalibrator_, Timer* timer_)
            : imuQuery(imuQuery_),
              imuId(imuId_),
              gravityCalibrator(gravityCalibrator_),
              timer(timer_)
        {}

        int SingleImuNavigation::navigate(unsigned int nTestQueries,
                                          float queryDelay, int runId, Logger log)
        {
            // File setup
            bool writeToFile = (sys.imu_id > 0) || (sys.run_id > 0);
            std::ofstream outfile;

            // IMU data logger
            ImuDataLogger imuDataLogger(outfile);
            imuDataLogger.setup(imuId, runId);

            // Calibrate gravitational acceleration
            NavigationVector gVector = gravityCalibrator.calibrate(imuQuery, timer);
            log.INFO("SINGLE_IMU", "Calibration complete, measuring.");

            // Return measured gravity vector
            log.INFO("SINGLE_IMU", "Measured gravity vector:\n\tgx=%+6.3f\tgy=%+6.3f\tgz=%+6.3f\n\n",
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

                // Intergrate
                velIntegrator.update(accCor);
                posIntegrator.update(vel);

                // Output values
                log.INFO("SINGLE_IMU", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
                        accCor.value[0], accCor.value[1], accCor.value[2], 
                        vel.value[0], vel.value[1], vel.value[2], 
                        pos.value[0], pos.value[1], pos.value[2]);		

                if (writeToFile > 0) imuDataLogger.dataToFile(&accRaw, &accCor, &vel, &pos);
                sleep(queryDelay);
            }

            if (writeToFile) outfile.close();
            return 0;
        }
    }
}
