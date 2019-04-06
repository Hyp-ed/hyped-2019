/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 3/4/19
 * Description: Demo for MPU9250 sensor using imu_manager
 * Troubleshooting:
 * If a single sensor does not initialise, try reconfiguring the chip_select_ pin.
 *  The GPIO pin may be faulty, so attempt with another pin and change
 *  chip_select_ pin list in imu_manager.cpp accordingly.
 *
 *    Copyright 2019 HYPED
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

#include "sensors/bms_manager.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::sensors::BmsManager;
using namespace hyped::data;
using namespace std;
using hyped::sensors::BmsManager;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);
  Data& data_ = Data::getInstance();
  BmsManager bms_manager(log);
  bms_manager.start();
  Thread::sleep(500);
  StateMachine state_machine = data_.getStateMachineData();
  state_machine.current_state = State::kAccelerating;
  data_.setStateMachineData(state_machine);
  Batteries batteries = data_.getBatteriesData();

  // change to accel state

  log.INFO("TEST-Bms", "Bms instance successfully created");
  for (int i = 0; i < 50; i++) {
    batteries = data_.getBatteriesData();
    auto lp = batteries.low_power_batteries[0];
    auto hp = batteries.high_power_batteries[0];
    log.INFO("TEST-BMSLP", "V = %u, C = %dmA, temp  = %d", lp.voltage, lp.current, lp.temperature);
    log.INFO("TEST-BMSHP", "V = %u, C = %dmA, temp  = %d", hp.voltage, hp.current, hp.temperature);
    Thread::sleep(500);
  }
 	return 0;
}