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

#include "sensors/fake_batteries.hpp"

#include "utils/timer.hpp"
#include "data/data.hpp"

namespace hyped {

using data::StripeCounter;

namespace sensors {

FakeBatteries::FakeBatteries(Logger& log, bool is_lp, bool is_fail)
    : data_(Data::getInstance()),
      is_started_(false),
      is_lp_(is_lp),
      is_fail_(is_fail)
{
  int fail_index = 0;
  if (is_fail_) fail_index = 1;
  if (is_lp_) {
    case_index_ = 0 + fail_index;
  } else {
    case_index_ = 2 + fail_index;
  }
  voltage_ = cases_[case_index_][0];
  current_ = cases_[case_index_][1];
  charge_ = cases_[case_index_][2];
  temperature_ = cases_[case_index_][3];
  low_voltage_cell_ = cases_[case_index_][4];
  high_voltage_cell_ = cases_[case_index_][5];
}

void FakeBatteries::getData(BatteryData* battery)
{
  if (is_lp_) {
    battery->voltage           = voltage_;
    battery->current           = current_;
    battery->charge            = charge_;
    battery->temperature       = temperature_;
    battery->low_voltage_cell  = low_voltage_cell_;
    battery->high_voltage_cell = high_voltage_cell_;
  } else {
    battery->voltage           = voltage_;
    battery->current           = current_;
    battery->charge            = charge_;
    battery->temperature       = temperature_;
    battery->low_voltage_cell  = low_voltage_cell_;
    battery->high_voltage_cell = high_voltage_cell_;
  }
}

bool FakeBatteries::isOnline()
{
  return true;
}

}}  // namespace hyped::sensors
