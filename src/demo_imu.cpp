/*
 * Author:
 * Organisation: HYPED
 * Date:
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
#include "sensors/imu_manager.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
// #include <vector>

#define MANAGER 1

using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using namespace std;
using hyped::sensors::ImuManager;
#if !MANAGER
using hyped::data::ImuData;
#endif

/**
TODO(Greg): Test manager with one sensor: PASSED (extra sensors must be unplugged)
TODO(Greg): Test manager with two sensors: FAILED (initialises once second sensor is unplugged)

*/

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);

  #if MANAGER
  ImuManager imu_manager_(log);
  imu_manager_.start();
  Thread::sleep(500);
  DataPoint<array<ImuData, Sensors::kNumImus>> data_array_;
  #else
  // {117, 125, 123, 111, 112, 110, 20},
  Imu* sensorArray [Sensors::kNumImus] = {new Imu(log,  117, 0x08), 
								   new Imu(log, 125, 0x08), 
								   new Imu(log, 123, 0x08),
								   new Imu(log, 111, 0x08),
								   new Imu(log, 112, 0x08),
								   new Imu(log, 110, 0x08),
								   new Imu(log, 20, 0x08)};
	ImuData sensorDataArray [Sensors::kNumImus] = {ImuData(), ImuData(), ImuData(), ImuData(),
										  ImuData(), ImuData(), ImuData()};

  #endif

  log.INFO("TEST-Imu", "Imu instance successfully created");
  for (int i = 0; i < 20; i++) {
    #if !MANAGER
    for (int a = 0; a< Sensors::kNumImus; a++) {
      sensorArray[a]->getData(&sensorDataArray[a]);
    }
    #else
    Data data_;
    Sensors sensors_data_ = data_.getSensorsData();
    data_array_ = sensors_data_.imu;
    log.DBG("Debug", "zero");
    #endif
    log.DBG("Debug", "0");
    Thread::sleep(100);
    log.DBG("Debug", "1");
    for(int j = 0; j < Sensors::kNumImus; j++){
      #if MANAGER
      log.DBG("Debug", "2");
      log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", j, data_array_.value[j].acc[0], data_array_.value[j].acc[1], data_array_.value[j].acc[2]);
      log.DBG("Debug", "3");
      #else
      log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", j, sensorDataArray[j].acc[0], sensorDataArray[j].acc[1], sensorDataArray[j].acc[2]);
      Thread::sleep(30);
      #endif
    }
  }
 	return 0;
}
