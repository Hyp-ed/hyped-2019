/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 22/02/2019
 * Description: Simulate trajectory measurements using generated accelerations
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

#include "navigation/single_imu_simulation.hpp"

#include <string>
#include <queue>

using std::string;
using std::queue;

namespace hyped
{
    namespace navigation
    {
        SingleImuSimulation::SingleImuSimulation()
        {}

        void SingleImuSimulation::loadSimData(queue<DataPoint<NavigationVector>>* dataQueue,
                                              queue<int>* stripeCount,
                                              ifstream* accData, ifstream* posData,
                                              string accFname, string posFname,
                                              float refreshRate, float stddev, float stripeSep)
        {
            float t = 0.;
            float ax, ay, az;
            // Open files
            accData->open(accFname);
            posData->open(posFname);
            // Simulate Gaussian noise
            default_random_engine generator;
            normal_distribution<float> noise(0., stddev);
            // Add noise to each acceleration reading and store
            while (*accData >> ax) {
                ax += noise(generator);
                ay = noise(generator);
                az = noise(generator);
                // Convert time to microseconds
                dataQueue->push(DataPoint<NavigationVector>(t*1e6, NavigationVector({ax, ay, az})));
                // Increment time
                t += refreshRate;
            }
            // Prepare stripe counts
            float dx;
            while (*posData >> dx) {
                stripeCount->push(static_cast<int>(dx/stripeSep));
            }
            // Close files
            accData->close();
            posData->close();
        }

        int SingleImuSimulation::simulate(Logger& log)
        {
            System& sys = System::getSystem();
            bool writeToFile = sys.run_id > 0;

            /*
                Sim data setup
            */
            // sim parameters
            float refreshRate = 1./3000.;   // query frequency
            float stddev = 0.5;             // noise = 2*sensorVariance (empirical)
            float stripeSep = 100./3.281;   // stipe separation (100ft)

            // file properties
            ifstream accData, posData;
            string accFname = "sim_data/acceleration-3k.txt";
            string posFname = "sim_data/displacement-3k.txt";

            // for quick access in sim
            queue<DataPoint<NavigationVector>> dataQueue;
            queue<int> stripeCount;

            // the main event
            loadSimData(&dataQueue, &stripeCount,
                        &accData, &posData,
                        accFname, posFname,
                        refreshRate, stddev, stripeSep);

            // Output setup
            ofstream outfile;
            // IMU data logger
            ImuDataLogger imuDataLogger(&outfile);
            imuDataLogger.setup(sys.imu_id, sys.run_id);


            /*
                Simulate run
            */
            // Store measured/estimated values
            DataPoint<NavigationVector> acc(0., NavigationVector({0., 0., 0.}));
            DataPoint<NavigationVector> vel(0., NavigationVector({0., 0., 0.}));
            DataPoint<NavigationVector> pos(0., NavigationVector({0., 0., 0.}));

            // Integrate acceleration -> velocity -> position
            Integrator<NavigationVector> velIntegrator(&vel);
            Integrator<NavigationVector> posIntegrator(&pos);

            // Check stipe count
            int stripesSeen = 0;

            // Perform acceleration, speed and distance queries
            while (!dataQueue.empty()) {
                // ``Query'' sensor
                acc = dataQueue.front();
                dataQueue.pop();

                // Intergrate
                velIntegrator.update(acc);
                posIntegrator.update(vel);

                // Check if stripe has been passed
                if (stripeCount.front() != stripesSeen) {
                    std::cout << "updating stripe count..." << std::endl;
                    // Update position if we pass a stripe
                    stripesSeen = stripeCount.front();
                    pos.value = NavigationVector({stripesSeen*stripeSep, 0., 0.});
                }
                stripeCount.pop();

                // Output values
                log.INFO("MAIN", "a_x:%+6.3f a_y:%+6.3f a_z:%+6.3f\tv_x:%+6.3f v_y:%+6.3f  "
                         "v_z:%+6.3f\tp_x:%+6.3f p_y:%+6.3f p_z:%+6.3f s:%d\n",
                                acc.value[0], acc.value[1], acc.value[2],
                                vel.value[0], vel.value[1], vel.value[2],
                                pos.value[0], pos.value[1], pos.value[2],
                                stripesSeen);

                if (writeToFile > 0) imuDataLogger.dataToFileSimulation(&acc, &vel, &pos);
            }

            if (writeToFile) outfile.close();

            return 0;
        }

    }
}
