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
  typedef array<array<uint16_t, 6>, 2> BatteryCases;
  typedef array<uint16_t, 6> BatteryInformation;
 public:
  FakeBatteries(Logger& log, bool is_high_voltage, bool is_nominal, int case_index);
  void getData(BatteryData* battery) override;
  bool isOnline() override;

 private:
  void init();          // these are not used
  bool checkTime();

  Data& data_;
  bool is_started_;
  bool is_high_voltage_;
  uint64_t ref_time_;

  /** 
   * Nominal HP: (3 packs)
   * Voltage: 118.6 V
   * Max current discharge: 720 A
   * Capacity: 13.8 Ah
   * Operating temperature: -30 to 60 C
   * 
   * Nominal LP: (1 packs)
   * Voltage 25.2 V
   * Max current discharge: 60 A
   * Capacity: 9 Ah
   * Operating temperature: 40 C
   */
  BatteryInformation success_ = {1100, 200, 30, 75, 3300, 3600};
  BatteryInformation failure_ = {170, 200, 30, 75, 0, 0};
  BatteryCases cases_ = {success_, failure_};       // different success and fail cases

  uint16_t voltage_;
  int16_t current_;
  int8_t temperature_;
  uint8_t charge_;
  uint16_t low_voltage_cell_;     // TODO(Greg): number of cells?
  uint16_t high_voltage_cell_;
};
}}    // namespace hyped::sensors

#endif  // SENSORS_FAKE_BATTERIES_HPP_