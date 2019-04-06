/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Main file for navigation class.
 *
 *  Copyright 2019 HYPED
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *  except in compliance with the License. You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under
 *  the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "main.hpp"

namespace hyped {
namespace navigation {

Main::Main(uint8_t id, Logger& log_)
  : Thread(id, log_),
    log(log_)
{
  log.INFO("NAVIGATION", "Navigation initialising");
}

void Main::run()
{
  System& sys = System::getSystem();
  Timer timer;

  // Sensor setup
  // TODO(Neil) - change to IMU manager in multi-imu setup
  const int i2c = 45;
  Imu* imu = new Imu(log, i2c, 0x08);
  ImuData* imuData = new ImuData();
  ImuQuery imuQuery = ImuQuery(imu, imuData, &timer);

  // Calibrate sensors wrt gravity vector
  unsigned int nCalibrationQueries = 10000;
  GravityCalibrator gravityCalibrator(nCalibrationQueries);

  // Start single IMU navigation
  float queryDelay = 0.01;
  SingleImuNavigation singleImuNavigation(sys, imuQuery, sys.imu_id,
                      gravityCalibrator);
  singleImuNavigation.navigate(queryDelay, sys.run_id, log);
}
}}  // namespace hyped navigation
