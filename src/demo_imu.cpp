/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 6/03/19
 * Description: Demo for MPU9250 sensor
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::data::ImuData;



int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);
  Imu imu(log, 66, 0x08);
  int data;

  log.INFO("TEST-mpu9260", "MPU9250 instance successfully created");

  

  for (int i=0; i< 100; i++) {
    imu.getTemperature(&data);
    log.DBG("TEST-mpu9250", "Imu temp %u degrees C", data);
    Thread::sleep(500);
  }


 	return 0;
}
