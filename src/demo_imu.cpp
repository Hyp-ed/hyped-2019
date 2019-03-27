
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


// #include "sensors/imu.hpp"
#include "sensors/imu_manager.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
// #include <vector>

// using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using namespace std;
// using hyped::data::ImuData;
using hyped::sensors::ImuManager;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);
  // Imu the_imu(log, 20, 0x08);
  DataPoint<array<ImuData, Sensors::kNumImus>> imu;
  ImuManager imu_manager_(log,&imu);        // use std::unique_ptr<ImuManagerInterface>   imu_manager_; ?
  imu_manager_.start();

  // ImuData imu;

  log.INFO("TEST-Imu", "Imu instance successfully created");
  for (int i = 0; i < 20; i++) {
    // the_imu.getData(&imu);
    Thread::sleep(100);
    for(int j = 0; j < Sensors::kNumImus; j++){
      log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", j, imu.value[j].acc[0], imu.value[j].acc[1], imu.value[j].acc[2]);    
      Thread::sleep(30);
    }
    // log.DBG("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 7, imu.acc[0], imu.acc[1], imu.acc[2]);    

  }
 	return 0;
}
