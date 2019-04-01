/*
 * Author: Gregory Dayao
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


namespace hyped {

using data::StripeCounter;
using utils::concurrent::Thread;
using utils::Logger;

namespace sensors {

FakeGpioCounter::FakeGpioCounter(Logger& log, bool miss_stripe, bool double_stripe)
    : log_(log),
    data_(Data::getInstance()),
    start_time_(0),
    check_time_(kCheckTime),
    miss_stripe_(miss_stripe),
    double_stripe_(double_stripe)
{
  stripe_count_.count.value = 0;                                      // start stripe count
  stripe_count_.operational = true;
}

FakeGpioCounter::FakeGpioCounter(Logger& log,
  bool miss_stripe, bool double_stripe, std::string file_path)
    : log_(log),
    data_(Data::getInstance()),
    start_time_(0),
    check_time_(kCheckTime),
    miss_stripe_(miss_stripe),
    double_stripe_(double_stripe),
    file_path_(file_path)
{
  stripe_count_.count.value = 0;                                      // start stripe count
  stripe_count_.operational = true;
  stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
  readFromFile(stripe_data_);           // read text from file into vector class member
}

StripeCounter FakeGpioCounter::getStripeCounter()     // returns incorrect stripe count
{
    data::Navigation nav   = data_.getNavigationData();
    uint32_t current_count = stripe_count_.count.value;

    int nav_count = std::floor(nav.distance/kStripeDistance);      // cast floor int;

    if (current_count != nav_count) {
      stripe_count_.count.value = nav_count;
      stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
    }

  return stripe_count_;
}

void FakeGpioCounter::checkData()
{
  // let pod wait at first...then start comparing data
  if (stripe_count_.count.timestamp - start_time_ > 5000000) Thread::sleep(300);
  if (miss_stripe_) {
    log_.INFO("fake_gpio_counter", "missed stripe, changing now");
    stripe_count_.count.value++;
    miss_stripe_ = false;
  } else if (double_stripe_) {
    log_.INFO("fake_gpio_counter", "missed stripe, changing now");
    stripe_count_.count.value--;
    double_stripe_ = false;
  }
}

bool FakeGpioCounter::timeCheck()             // used to see if it is time to check
{
  if (utils::Timer::getTimeMicros() - stripe_count_.count.timestamp >= kCheckTime) {
    return true;
  }
  return false;
}

void FakeGpioCounter::readFromFile(std::vector<StripeCounter>& data)
  {
    std::ifstream data_file(file_path_, std::ifstream::in);
    float count;
    float time;
    if (data_file.is_open()) {
      // read in pairs of stripe_count, timestamp
      while (data_file >> count && data_file >> time) {
        StripeCounter this_line;
        this_line.count.value = count;
        this_line.count.timestamp = time;
        data.push_back(this_line);
      }
    } else {
      log_.ERR("fake_gpio_counter", "cannot open file");
    }
    data_file.close();
  }

  void FakeGpioCounter::readData()
  {
    stripe_count_.count.value = stripe_data_.front().count.value;
    stripe_count_.count.value = stripe_data_.front().count.timestamp;
    stripe_data_.erase(stripe_data_.begin());
  }

}}
