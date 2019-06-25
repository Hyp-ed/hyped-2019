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

constexpr int kHPSSR = 70;
constexpr int kLPSSR = 71;

namespace hyped {
namespace sensors {

BmsManager::BmsManager(Logger& log)
    : ManagerInterface(log),
      sys_(utils::System::getSystem()),
      data_(Data::getInstance())
{
  old_timestamp_ = utils::Timer::getTimeMicros();

  if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
    // create BMS LP
    for (int i = 10; i < 10 + data::Batteries::kNumLPBatteries; i++) {
      BMS* bms = new BMS(i, log_);
      bms->start();
      bms_[i] = bms;
    }
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + 10 + data::Batteries::kNumLPBatteries] = new BMSHP(i, log_);
    }
    // Set SSR switches for real system
    kill_hp_ = new GPIO(kHPSSR, utils::io::gpio::kOut);
    kill_lp_ = new GPIO(kLPSSR, utils::io::gpio::kOut);
    kill_hp_->set();
    kill_lp_->set();    // kHPSSR and kLPSSR set low if no power to BBB
    log_.INFO("BMS-MANAGER", "HP SSR %d has been set", kHPSSR);
    log_.INFO("BMS-MANAGER", "LP SSR %d has been set", kLPSSR);
  } else if (sys_.fake_batteries_fail) {
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

        // set low to kHPSSR if batteries is kCriticalFailure
        // batteries module status forces kEmergencyBraking, which actuates embrakes
        kill_hp_->clear();
        log_.INFO("BMS-MANAGER", "Batteries Critical! HP SSR cleared");
      }
    }
    // publish the new data
    data_.setBatteriesData(batteries_);

    // set low to kHPSSR if pod in emergency states
    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kEmergencyBraking || state == data::State::kFailureStopped) {
      kill_hp_->clear();
      log_.INFO("BMS-MANAGER", "Emergency State! HP SSR cleared");
    }
    sleep(100);
  }
}

bool BmsManager::batteriesInRange()
{
  // check LP
  for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
    auto& battery = batteries_.low_power_batteries[i];      // reference batteries individually
    if (battery.voltage < 175 || battery.voltage > 294) {   // voltage in 17.5V to 29.4V
      log_.ERR("BMS-MANAGER", "BMS LP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 50 || battery.current > 500) {       // current in 5A to 50A
      log_.ERR("BMS-MANAGER", "BMS LP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.temperature < 10 || battery.temperature > 60) {  // temperature in 10C to 60C
      log_.ERR("BMS-MANAGER", "BMS LP %d temperature out of range: %d", i, battery.temperature);
      return false;
    }

    if (battery.charge < 20 || battery.charge > 100) {  // charge in 20% to 100%
      log_.ERR("BMS-MANAGER", "BMS LP %d charge out of range: %d", i, battery.charge);
      return false;
    }
  }

  // check HP
  for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
    auto& battery = batteries_.high_power_batteries[i];     // reference battereis individually
    if (battery.voltage < 1000 || battery.voltage > 1296) {   // voltage in 100V to 129.6V
      log_.ERR("BMS-MANAGER", "BMS HP %d voltage out of range: %d", i, battery.voltage);
      return false;
    }

    if (battery.current < 0 || battery.current > 3500) {  // current in 0A to 350A
      log_.ERR("BMS-MANAGER", "BMS HP %d current out of range: %d", i, battery.current);
      return false;
    }

    if (battery.temperature < 10 || battery.temperature > 65) {  // temperature in 10C to 65C
      log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.temperature);
      return false;
    }

    // TODO(Greg): HP Charge scaling needs to be tested
    if (battery.charge < 20 || battery.charge > 100) {  // charge in 20% to 100%
      log_.ERR("BMS-MANAGER", "BMS HP %d charge out of range: %d", i, battery.charge);
      return false;
    }
  }
  return true;
}

}}  // namespace hyped::sensors
