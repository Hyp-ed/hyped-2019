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
#include "utils/timer.hpp"
#include "sensors/fake_batteries.hpp"
#include "utils/config.hpp"

namespace hyped {
namespace sensors {

BmsManager::BmsManager(Logger& log)
    : ManagerInterface(log),
      sys_(utils::System::getSystem()),
      data_(Data::getInstance()),
      hp_ssr_(sys_.config->sensors.HPSSR),
      lp_ssr_(sys_.config->sensors.LPSSR),
      pin_imd_ {46, 63, 33, 88, 11, 80},
      pin_led_ {76, 77}
{
  old_timestamp_ = utils::Timer::getTimeMicros();

  if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
    // create BMS LP
    for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
      BMS* bms = new BMS(i, log_);
      bms->start();
      bms_[i] = bms;
    }
    // create BMS HP
    for (int i = 0; i < data::Batteries::kNumHPBatteries; i++) {
      bms_[i + data::Batteries::kNumLPBatteries] = new BMSHP(i, log_);
    }
    // Set SSR switches for real system

    kill_hp_ = new GPIO(hp_ssr_, utils::io::gpio::kOut);
    kill_lp_ = new GPIO(lp_ssr_, utils::io::gpio::kOut);
    kill_hp_->set();
    kill_lp_->set();
    log_.INFO("BMS-MANAGER", "HP SSR %d has been set", hp_ssr_);
    log_.INFO("BMS-MANAGER", "LP SSR %d has been set", lp_ssr_);

    // TODO(Greg): confirm pin selection with electronics team
    for (int i = 0; i < kNumImd; i++) {
      imd_[i] = new GPIO(pin_imd_[i], utils::io::gpio::kIn);
    }
    for (int i = 0; i < kNumLED; i++) {
      green_led_[i] = new GPIO(pin_led_[i], utils::io::gpio::kOut);
      green_led_[i]->set();
    }
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

    // iterate through imd_ and set LEDs
    for (GPIO* pin : imd_) {
      uint8_t val = pin->read();     // will check every cycle of run()
      if (val) {
        for (int i = 0; i < kNumLED; i++) {
          green_led_[i]->clear();
          log_.ERR("BMS-MANAGER", "IMD short! Green LED %d cleared", i);
        }
      }
    }

    // check health of batteries
    if (batteries_.module_status != data::ModuleStatus::kCriticalFailure) {
      if (!batteriesInRange()) {
        log_.ERR("BMS-MANAGER", "battery failure detected");
        batteries_.module_status = data::ModuleStatus::kCriticalFailure;
        if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
          kill_hp_->clear();
          log_.ERR("BMS-MANAGER", "Batteries Critical! HP SSR cleared");
        }
      }
    }
    // publish the new data
    data_.setBatteriesData(batteries_);

    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kEmergencyBraking || state == data::State::kFailureStopped) {
      if (!(sys_.fake_batteries || sys_.fake_batteries_fail)) {
        kill_hp_->clear();
        log_.ERR("BMS-MANAGER", "Emergency State! HP SSR cleared");
      }
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

    if (battery.average_temperature < 10 || battery.average_temperature > 60) {  // NOLINT[whitespace/line_length] temperature in 10C to 60C
      log_.ERR("BMS-MANAGER", "BMS LP %d temperature out of range: %d", i, battery.average_temperature); // NOLINT[whitespace/line_length]
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

    if (battery.average_temperature < 10 || battery.average_temperature > 65) {  // NOLINT[whitespace/line_length] temperature in 10C to 65C
      log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.average_temperature); // NOLINT[whitespace/line_length]
      return false;
    }

    if (battery.low_temperature < 10) {
      log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.low_temperature); // NOLINT[whitespace/line_length]
      return false;
    }

    if (battery.high_temperature > 65) {
      log_.ERR("BMS-MANAGER", "BMS HP %d temperature out of range: %d", i, battery.high_temperature); // NOLINT[whitespace/line_length]
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
