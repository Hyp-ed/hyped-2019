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

FakeBatteries::FakeBatteries(Logger& log, bool is_high_voltage, bool is_nominal, int case_index)
    : data_(Data::getInstance()),
      is_started_(false),
      is_high_voltage_(is_high_voltage),
      voltage_(cases_[case_index][0]),
      current_(cases_[case_index][1]),
      temperature_(cases_[case_index][2]),
      charge_(cases_[case_index][3]),
      low_voltage_cell_(cases_[case_index][4]),
      high_voltage_cell_(cases_[case_index][5])
{}

void FakeBatteries::getData(BatteryData* battery)
{
  battery->voltage           = voltage_;
  battery->temperature       = temperature_;
  battery->current           = current_;
  battery->charge            = charge_;
  battery->low_voltage_cell  = low_voltage_cell_;
  battery->high_voltage_cell = high_voltage_cell_;
}

bool FakeBatteries::isOnline()
{
  return true;
}

}}  // namespace hyped::sensors
