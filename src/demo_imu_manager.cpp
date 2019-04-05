
/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 27/03/19
 * Description: Demo for imu manager
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

#include "sensors/imu_manager.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using namespace std;
using hyped::sensors::ImuManager;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);
  ImuManager imu_manager_(log);
  imu_manager_.start();
  Data& data = Data::getInstance();

  Sensors sensors = data.getSensorsData();

  auto state = data.getStateMachineData();
  state.current_state = State::kAccelerating;
  data.setStateMachineData(state);

  log.INFO("TEST-Imu", "Imu instance successfully created");
  for (int i = 0; i < 20; i++) {
    sensors = data.getSensorsData();
    for(int j = 0; j < Sensors::kNumImus; j++){
      log.INFO("TEST-Imu", "Imu %d readings: %f m/s^2, y: %f m/s^2, z: %f m/s^2", j, sensors.imu.value[j].acc[0], sensors.imu.value[j].acc[1], sensors.imu.value[j].acc[2]); // NOLINT
    }
    Thread::sleep(100);
  }
 	return 0;
}