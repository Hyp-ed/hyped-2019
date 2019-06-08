/*
 * Author: Gregory Dayao and Jack Horsburgh
 * Organisation: HYPED
 * Date: 1/4/19
 * Description:
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "sensors/fake_gpio_counter.hpp"
#include "utils/timer.hpp"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

uint64_t kBrakeTime = 10000000;
uint32_t kTrackDistance = 2000;
double kStripeDistance = 30.48;     // metres
uint64_t kCheckTime = 358588;
uint64_t kMaxTime = 1500;     // between stripe readings before throw failure


namespace hyped {

using data::StripeCounter;
using utils::concurrent::Thread;
using utils::Logger;

namespace sensors {

FakeGpioCounter::FakeGpioCounter(Logger& log, bool miss_stripe)
    : log_(log),
    data_(Data::getInstance()),
    start_time_(0),
    miss_stripe_(miss_stripe),
    // double_stripe_(double_stripe),
    is_from_file_(false),
    acc_ref_init_(false)
{
  stripe_count_.count.value = 0;                                      // start stripe count
  stripe_count_.operational = true;
  if (miss_stripe_) {
    log_.INFO("Fake-GpioCounter", "Fake Keyence Fail initialised");
  } else {
    log_.INFO("Fake-GpioCounter", "Fake Keyence initialised");
  }
}

FakeGpioCounter::FakeGpioCounter(Logger& log,
  bool miss_stripe, std::string file_path)
    : log_(log),
    data_(Data::getInstance()),
    start_time_(0),
    miss_stripe_(miss_stripe),
    // double_stripe_(double_stripe),
    file_path_(file_path),
    is_from_file_(true),
    acc_ref_init_(false)
{
  stripe_count_.count.value = 0;                                      // start stripe count
  stripe_count_.operational = true;
  stripe_count_.count.timestamp = 0;
  readFromFile(stripe_data_);           // read text from file into vector class member
}

StripeCounter FakeGpioCounter::getStripeCounter()     // returns incorrect stripe count
{
  data::State state = data_.getStateMachineData().current_state;
  if (!acc_ref_init_ && state == data::State::kAccelerating) {
    accel_ref_time_ = utils::Timer::getTimeMicros();
    acc_ref_init_ = true;
  }

  if (is_from_file_) {
    // Get time in micro seconds and iterate through the vector until we find what stripe we are at
    uint64_t time_now_micro = (utils::Timer::getTimeMicros() - accel_ref_time_)/1000;
    for (StripeCounter stripe : stripe_data_) {
      if (stripe.count.timestamp < time_now_micro) {
        stripe_count_.count.value = stripe.count.value;
        stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
      } else {
        break;
      }
    }
    checkData();
  } else {
    data::Navigation nav   = data_.getNavigationData();     // throw failure from fake_imu
    uint32_t current_count = stripe_count_.count.value;

    uint16_t nav_count = std::floor(nav.distance/kStripeDistance);      // cast floor int;

    if (current_count != nav_count) {
      stripe_count_.count.value = nav_count;
      stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
    }
  }

  return stripe_count_;
}

void FakeGpioCounter::checkData()
{
  if (is_from_file_) {
    uint64_t time_after = ((utils::Timer::getTimeMicros() - accel_ref_time_)/1000) - stripe_count_.count.timestamp;   // NOLINT [whitespace/line_length]
    if (time_after > kMaxTime && miss_stripe_) {  // TODO(Jack,Greg): Change max time bw stripes
      log_.INFO("FakeGpioCounter", "missed stripe!");
      // throw failure to keyence, override with nav data
      stripe_count_.operational = false;
    }
  }
  // let pod wait at first...then start comparing data
  // if (((utils::Timer::getTimeMicros() - accel_ref_time_)/1000) > 8000) {
  //   if (miss_stripe_) {
  //     log_.INFO("FakeGpioCounter", "missed stripe, changing now");
  //     stripe_count_.count.value++;
  //   }
    // else if (double_stripe_) {
    //   log_.INFO("FakeGpioCounter", "double stripe count, changing now");
    //   stripe_count_.count.value--;
    // }
  // }
}

void FakeGpioCounter::readFromFile(std::vector<StripeCounter>& data)
  {
    std::ifstream data_file(file_path_, std::ifstream::in);
    float count;
    float time;
    if (data_file.is_open()) {
      // read in pairs of stripe_count, timestamp
      while (data_file >> time && data_file >> count) {
        StripeCounter this_line;
        this_line.count.value = count;
        this_line.count.timestamp = time;
        stripe_data_.push_back(this_line);
      }
    } else {
      log_.ERR("FakeGpioCounter", "cannot open file");
    }
    data_file.close();
  }
}}
