/*
 * Author: Jack Horsburgh and Gregory Dayo
 * Organisation: HYPED
 * Date: 6/04/19
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

#include <stdlib.h>

#include "utils/timer.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"

namespace hyped {

namespace sensors {

FakeBatteries::FakeBatteries(Logger& log, bool is_lp, bool is_fail)
      : data_(Data::getInstance()),
      log_(log),
      is_started_(false),
      is_lp_(is_lp),
      is_fail_(is_fail),
      acc_started_(false),
      acc_start_time_(0),
      failure_time_(0),
      failure_happened_(false)
{
  if (is_lp_) {
    case_index_ = 1;
  } else {
    case_index_ = 3;
  }
  updateBatteryData();
}

void FakeBatteries::getData(BatteryData* battery)
{
  // We want to fail after we start accelerating
  // We can make it random from 0 to 20 seconds
  if (!acc_started_) {
    data::State state = data_.getStateMachineData().current_state;
    if (state == data::State::kAccelerating) {
      acc_start_time_ = utils::Timer::getTimeMicros();
      // Generate a random time for a failure
      if (is_fail_)
        failure_time_ = (rand() % 20 + 1) * 1000000;
      acc_started_ = true;
    }
  }
  checkFailure();
  battery->voltage           = voltage_;
  battery->current           = current_;
  battery->charge            = charge_;
  battery->temperature       = temperature_;
  battery->low_voltage_cell  = low_voltage_cell_;
  battery->high_voltage_cell = high_voltage_cell_;
}

void FakeBatteries::checkFailure()
{
  if (is_fail_ && failure_time_ != 0 && !failure_happened_) {
    if (utils::Timer::getTimeMicros() - acc_start_time_ >= failure_time_) {
      case_index_--;
      updateBatteryData();
      failure_happened_ = true;
    }
  }
}

void FakeBatteries::updateBatteryData()
{
  voltage_            = cases_[case_index_][0];
  current_            = cases_[case_index_][1];
  charge_             = cases_[case_index_][2];
  temperature_        = cases_[case_index_][3];
  low_voltage_cell_   = cases_[case_index_][4];
  high_voltage_cell_  = cases_[case_index_][5];
}

bool FakeBatteries::isOnline()
{
  return true;
}

}}  // namespace hyped::sensors
