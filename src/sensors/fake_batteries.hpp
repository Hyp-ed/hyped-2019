/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Main class for fake.
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

#ifndef SENSORS_FAKE_BATTERIES_HPP_
#define SENSORS_FAKE_BATTERIES_HPP_

#include <string>
#include <vector>

#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "sensors/interface.hpp"

namespace hyped {

using utils::Logger;
using data::Data;

namespace sensors {


class FakeBatteries : public BMSInterface {
  typedef array<array<uint16_t, 6>, 4> BatteryCases;
  typedef array<uint16_t, 6> BatteryInformation;
 public:
  FakeBatteries(Logger& log, bool is_lp, bool is_high_voltage);
  void getData(BatteryData* battery) override;
  bool isOnline() override;

 private:
  Data& data_;
  bool is_started_;

  /*
// check LP
    if (battery.voltage < 140 || battery.voltage > 252)             // voltage in 14V to 25.2V
    if (battery.current < 0 || battery.current > 300)               // current in 0A to 30A
    if (battery.temperature < -20 || battery.temperature > 70)      // temperature in -20C to 70C

  // check HP
    if (battery.voltage < 720 || battery.voltage > 1246)            // voltage in 72V to 124.6V
    if (battery.current < -4000 || battery.current > 13500)         // current in -400A to 1350A
    if (battery.temperature < -20 || battery.temperature > 70)      // temperature in -20C to 70C

  struct BatteryData {
  uint16_t  voltage;  // V
  int16_t   current;  // mA
  uint8_t   charge;
  int8_t    temperature;  // C
  uint16_t  low_voltage_cell;  // V
  uint16_t  high_voltage_cell;  // V
  };
  */

  BatteryInformation lp_failure_ = {10, 5000, 0, 100, 100, 500};
  BatteryInformation lp_success_ = {252, 200, 0, 50, 140, 252};    // TODO(Greg): adjust values
  BatteryInformation hp_failure_ = {170, 20000, 0, 100, 200, 2000};
  BatteryInformation hp_success_ = {1000, 10000, 0, 50, 720, 1246};    // TODO(Greg): adjust values
  BatteryCases cases_ = {lp_failure_, lp_success_, hp_failure_, hp_success_};       // different success and fail cases

  bool is_lp_;
  bool is_fail_;
  int case_index_;

  uint16_t voltage_;
  int16_t current_;
  uint8_t charge_;
  int8_t temperature_;
  uint16_t low_voltage_cell_;
  uint16_t high_voltage_cell_;
};
}}    // namespace hyped::sensors

#endif  // SENSORS_FAKE_BATTERIES_HPP_
