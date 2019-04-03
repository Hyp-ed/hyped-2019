/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 20/06/18
 * Description:
 * BMS manager for getting battery data and pushes to data struct.
 * Checks whether batteries are in range and enters emergency state if fails.
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


#include "sensors/bms_manager.hpp"

#include "sensors/bms.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"

namespace hyped {
namespace sensors {

BmsManager::BmsManager(Logger& log)   // TODO(Greg): these are initialised in the manager and data written to struct from there
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
  } else {
    // fake batteries here
  }

  // initialise batteries data
  if (updated()) {
    batteries_.high_power_batteries = hp_batteries_;
    batteries_.low_power_batteries = lp_batteries_;
    data_.setBatteriesData(batteries_);
  }
  Thread::yield();
  log_.INFO("BMS-MANAGER", "batteries data has been initialised");
}

void BmsManager::run()
{
  while (sys_.running_) {
    // keep updating data_ based on values read from sensors
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      bms_[i]->getData(&((lp_batteries_)[i]));
      if (!bms_[i]->isOnline()) (lp_batteries_)[i].voltage = 0;
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries]->getData(&(hp_batteries_)[i]);
      if (!bms_[i + data::Batteries::kNumLPBatteries]->isOnline()) (hp_batteries_)[i].voltage = 0;
    }

    if (updated()) {
      resetTimestamp();       // of battery manager
      // check health of batteries
      if (batteries_.module_status != data::ModuleStatus::kCriticalFailure) {
        if (!batteriesInRange()) {
          log_.ERR("BMS-MANAGER", "battery failure detected");
          batteries_.module_status = data::ModuleStatus::kCriticalFailure;
        }
      }
      // publish the new data
      data_.setBatteriesData(batteries_);
    }
    Thread::yield();
    timestamp = utils::Timer::getTimeMicros();
    sleep(100);
  }
}

bool BmsManager::updated()
{
  if (old_timestamp_ != timestamp) {
    return true;
  }
  return false;
}

void BmsManager::resetTimestamp()
{
  old_timestamp_ = timestamp;
}

bool BmsManager::batteriesInRange()
{
  // TODO(Greg): Check these values with power team
  // check all LP and HP battery values are in expected range

  // check LP
  for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
    auto& battery = batteries_.low_power_batteries[i];      // reference batteries individually
    if (battery.voltage < 140 || battery.voltage > 252) {   // voltage in 14V to 25.2V
      log_.ERR("BMS-MANAGER", "BMS LP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 300) {       // current in 0A to 30A
      log_.ERR("BMS-MANAGER", "BMS LP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.temperature < -20 || battery.temperature > 70) {  // temperature in -20C to 70C
      log_.ERR("BMS-MANAGER", "BMS LP %d temperature out of range: %d", i, battery.temperature);
      return false;
    }
  }

  // check HP
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    auto& battery = batteries_.high_power_batteries[i];     // reference battereis individually
    if (battery.voltage < 720 || battery.voltage > 1246) {   // voltage in 72V to 124.6V
      log_.ERR("BMS-MANAGER", "BMS HP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < -4000 || battery.current > 13500) {       // current in -400A to 1350A
      log_.ERR("BMS-MANAGER", "BMS HP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.temperature < -20 || battery.temperature > 70) {  // temperature in -20C to 70C
      log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.temperature);
      return false;
    }
  }
  return true;
}

}}  // namespace hyped::sensors
