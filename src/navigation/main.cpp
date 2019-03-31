/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Main file for navigation class.
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

#include "main.hpp"

namespace hyped {

    namespace navigation {

        int main(int argc, char *argv[])
        {
            // System setup
            System::parseArgs(argc, argv);
            System& sys = System::getSystem();
            Logger log(sys.verbose, sys.debug);
            Timer timer;

            // Sensor setup
            const int i2c = 66;
            Imu* imu = new Imu(log, i2c, 0x08);
            ImuData* imuData = new ImuData();
            ImuQuery imuQuery = ImuQuery(imu, imuData, &timer);

            unsigned int nCalibrationQueries = 10000;
            unsigned int nTestQueries = 50000;
            float queryDelay = 0.01;

            // Gravity calibrator
            GravityCalibrator gravityCalibrator(nCalibrationQueries);

            // Start single IMU navigation
            SingleImuNavigation singleImuNavigation(imuQuery, sys.imu_id,
                                                    gravityCalibrator, &timer);
            return singleImuNavigation.navigate(nTestQueries, queryDelay, sys.run_id, log);
        }
    }
}
