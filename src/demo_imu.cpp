
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
#include <vector>

using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using namespace std;
using hyped::data::ImuData;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);
  Imu imu(log, 66, 0x08);

  log.INFO("TEST-Imu", "MPU9250 instance successfully created");
  int i = 0;
  while (i < 20) {
    std::vector<ImuData> data;
    int count = imu.readFifo(data);
    log.DBG("ReadFifo Count", "%d", count);
    for (int i=0; i < count; i++) {
      log.DBG("TEST-mpu9250", "accelerometer readings x: %f m/s^2, y: %f m/s^2, z: %f m/s^2", data[i].acc[0], data[i].acc[1], data[i].acc[2]);    
    }
    Thread::sleep(10);
    i++;
  }
 	return 0;
}
