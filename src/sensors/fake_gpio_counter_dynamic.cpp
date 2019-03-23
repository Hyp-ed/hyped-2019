/*
 * Author:
 * Organisation: HYPED
 * Date:
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
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

#include "sensors/fake_gpio_counter_dynamic.hpp"
#include "utils/timer.hpp"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

constexpr uint64_t kBrakeTime = 10000000;
constexpr uint32_t kTrackDistance = 2000;
constexpr uint32_t kStripeDistance = 30.48;     // metres

namespace hyped {

using data::StripeCounter;
using hyped::utils::concurrent::Thread;
using sensors::FakeGpioCounterInterface;

namespace sensors {

FakeGpioCounterDynamic::FakeGpioCounterDynamic(Logger& log, bool miss_stripe, bool double_stripe)
    : log_(log),
      data_(Data::getInstance()),
      start_time_(0),       // TODO(Greg): fix static variable errors from interface
      check_time_(358588),
      miss_stripe_(miss_stripe),
      double_stripe_(double_stripe)
{
  stripe_count_.count.value = 0;                                      // start stripe count
  stripe_count_.operational = true;
  init_ = true;
}

StripeCounter FakeGpioCounterDynamic::getData()     // returns incorrect stripe count
{
  if(init_) {
    stripe_count_.count.timestamp = utils::Timer::getTimeMicros();      // the start time
    start_time_ = stripe_count_.count.timestamp;
    init_ = false;
  }

  if (timeCheck()) {    // only do this if it's time to check
    data::Navigation nav   = data_.getNavigationData();
    uint32_t current = stripe_count_.count.value;
    
    int nav_distance = int (nav.distance/kStripeDistance);      // cast int;

    if (nav_distance == current) {             // if correct
      log_.INFO("fake_gpio_counter", "correct count");
    } else if (nav.distance < current) {       // counted too many stripes
      log_.INFO("fake_gpio_counter", "incorrect count, substracting stripe");
      double_stripe_ = true;
    } else if (nav.distance > current) {      // if missed extra_count number of stripes
      
      log_.INFO("fake_gpio_counter", "incorrect count, %d stripes missed", nav.distance-current);
      miss_stripe_ = true;
    }
      log_.INFO("fake_gpio_counter", "nav.distance=%f, new_count=%d, timestamp=%f",
                  nav.distance, stripe_count_.count.value, stripe_count_.count.timestamp);
  }
  return stripe_count_;
}

void FakeGpioCounterDynamic::checkData()
{
  // let pod wait at first...then start comparing data
  if (stripe_count_.count.timestamp - start_time_ > 5000000) Thread::sleep(300);
  if (miss_stripe_) {
    log_.INFO("fake_gpio_counter", "missed stripe, changing now");
    stripe_count_.count.value++;
    miss_stripe_ = false;
  }
  else if(double_stripe_) {
    log_.INFO("fake_gpio_counter", "missed stripe, changing now");
    stripe_count_.count.value--;
    double_stripe_ = false;
  }
}

bool FakeGpioCounterDynamic::timeCheck()             // used to see if it is time to check
{
  if (utils::Timer::getTimeMicros() - stripe_count_.count.timestamp >= check_time_) {
    return true;
  }
  return false;
}

}}
