/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 30/3/19
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
Update 31/3: Data will be duplicates when run 2 sensors with one unplugged- unreliable data!
Update 1/4: all sensors are functional individually through imu_manager.
Update 1/4: Run each sensor individually- fails at moment sensor is plugged in
*/

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);

  #if MANAGER
  DataPoint<array<ImuData, Sensors::kNumImus>> data_array_;
  Data& data_ = Data::getInstance();
  ImuManager imu_manager_(log);
  imu_manager_.start();
  Thread::sleep(500);
  
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

  // Imu imu0(log, 20, 0x08);
  // Imu imu1(log, 110, 0x08);
  // Imu imu2(log, 112, 0x08);
  // Imu imu3(log, 111, 0x08);
  // Imu imu4(log, 123, 0x08);
  // Imu imu5(log, 125, 0x08);
  // Imu imu6(log, 117, 0x08);

  // ImuData data0;
  // ImuData data1;
  // ImuData data2;
  // ImuData data3;
  // ImuData data4;
  // ImuData data5;
  // ImuData data6;

  // for (int i = 0; i<50; i++) {
  //   imu0.getData(&data0);
  //   log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 0, data0.acc[0], data0.acc[1], data0.acc[2]);
  //   imu1.getData(&data1);
  //   log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 1, data1.acc[0], data1.acc[1], data1.acc[2]);
  //   imu2.getData(&data2);
  //   log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 2, data2.acc[0], data2.acc[1], data2.acc[2]);
  //   imu3.getData(&data3);
  //   log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 3, data3.acc[0], data3.acc[1], data3.acc[2]);
  //   imu4.getData(&data4);
  //   log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 4, data4.acc[0], data4.acc[1], data4.acc[2]);
  //   imu5.getData(&data5);
  //   log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 5, data5.acc[0], data5.acc[1], data5.acc[2]);
  //   imu6.getData(&data6);
  //   log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", 6, data6.acc[0], data6.acc[1], data6.acc[2]);
  // }
  #endif


  log.INFO("TEST-Imu", "Imu instance successfully created");
  for (int i = 0; i < 50; i++) {
    #if MANAGER
    data_array_ = data_.getSensorsImuData();
    #else
    for (int a = 0; a< Sensors::kNumImus; a++) {
      sensorArray[a]->getData(&sensorDataArray[a]);
    }
    #endif
    Thread::sleep(100);
    for (int j = 0; j < Sensors::kNumImus; j++) {
      #if MANAGER
      log.INFO("TEST-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", j, data_array_.value[j].acc[0], data_array_.value[j].acc[1], data_array_.value[j].acc[2]);
      #else
      log.INFO("Test-Imu", "accelerometer readings %d: %f m/s^2, y: %f m/s^2, z: %f m/s^2", j, sensorDataArray[j].acc[0], sensorDataArray[j].acc[1], sensorDataArray[j].acc[2]);
      Thread::sleep(30);
      #endif
    }
  }
 	return 0;
}
