/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 20/06/18
 * Description:
 * BMS manager for getting battery data and pushes to data struct.
 * Checks whether batteries are in range and enters emergency state if fails.
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

#include "sensors/bms.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"
#include "sensors/fake_batteries.hpp"

namespace hyped {
namespace sensors {

BmsManager::BmsManager(Logger& log)
    : ManagerInterface(log),
      sys_(utils::System::getSystem()),
      data_(Data::getInstance())
{
  old_timestamp_ = utils::Timer::getTimeMicros();

  if (!sys_.fake_batteries) {
    // create BMS LP
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      BMS* bms = new BMS(i, log_);
      bms->start();
      bms_[i] = bms;
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new BMSHP(i, log_);
    }
  } else if (sys_.fake_temperature_fail) {
    // fake batteries fail here
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i] = new FakeBatteries(log_, true, true);
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, true);
    }
  } else {
    // fake batteries here
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i] = new FakeBatteries(log_, true, false);
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new FakeBatteries(log_, false, false);
    }
  }
  // kInit for SM transition
  batteries_ = data_.getBatteriesData();
  batteries_.module_status = data::ModuleStatus::kInit;
  data_.setBatteriesData(batteries_);
  Thread::yield();
  log_.INFO("BMS-MANAGER", "batteries data has been initialised");
}

void BmsManager::run()
{
  while (sys_.running_) {
    // keep updating data_ based on values read from sensors
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i]->getData(&batteries_.low_power_batteries[i]);
      if (!bms_[i]->isOnline())
        batteries_.low_power_batteries[i].voltage = 0;
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries]->getData(&batteries_.high_power_batteries[i]);
      if (!bms_[i + data::Batteries::kNumLPBatteries]->isOnline())
        batteries_.high_power_batteries[i].voltage = 0;
    }
    // check health of batteries
    if (batteries_.module_status != data::ModuleStatus::kCriticalFailure) {
      if (!batteriesInRange()) {
        log_.ERR("BMS-MANAGER", "battery failure detected");
        batteries_.module_status = data::ModuleStatus::kCriticalFailure;
      }
    }
    // publish the new data
    data_.setBatteriesData(batteries_);
    sleep(100);
  }
}

bool BmsManager::batteriesInRange()
{
  // TODO(Greg): Check these values with power team
  // check all LP and HP battery values are in expected range

  // check LP
  for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
    auto& battery = batteries_.low_power_batteries[i];      // reference batteries individually
    if (battery.voltage < 240 || battery.voltage > 252) {   // voltage in 24V to 25.2V
      log_.ERR("BMS-MANAGER", "BMS LP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 120 || battery.current > 700) {       // current in 12A to 70A
      log_.ERR("BMS-MANAGER", "BMS LP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.temperature < 10 || battery.temperature > 50) {  // temperature in 10C to 50C
      log_.ERR("BMS-MANAGER", "BMS LP %d temperature out of range: %d", i, battery.temperature);
      return false;
    }
  }

  // check HP
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    auto& battery = batteries_.high_power_batteries[i];     // reference battereis individually
    if (battery.voltage < 1100 || battery.voltage > 1188) {   // voltage in 110V to 118.8V
      log_.ERR("BMS-MANAGER", "BMS HP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 4000) {  // current in 0A to 400A
      log_.ERR("BMS-MANAGER", "BMS HP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.temperature < 10 || battery.temperature > 50) {  // temperature in 10C to 50C
      log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.temperature);
      return false;
    }
  }
  return true;
}

}}  // namespace hyped::sensors
