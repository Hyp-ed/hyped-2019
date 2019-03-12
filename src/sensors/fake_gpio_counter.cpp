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

#include "sensors/fake_gpio_counter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include "utils/timer.hpp"
#include "data/data.hpp"

constexpr uint64_t kBrakeTime = 10000000;     // calculate this constant from FDP based of max acc, it is a const
constexpr uint32_t kTrackDistance = 2000;
constexpr uint32_t kStripeDistance = 30;          // every 30 meters, is this the right unit?

namespace hyped {

using data::StripeCounter;

namespace sensors {

FakeGpioCounter::FakeGpioCounter(Logger& log, bool miss_stripe)
    : log_(log),
      data_(Data::getInstance()),    // need to update data structures
      start_time_(0),                // just zero?
      ref_time_(start_time_),        // current time
      check_time_(3150000),          // 3.15 seconds
      brake_time_(kBrakeTime),                   
      // stripe_count_(0);
      miss_stripe_(miss_stripe),
      is_accelerating_(false)
{
  stripe_count_.count.timestamp = utils::Timer::getTimeMicros();      // the start time
  stripe_count_.count.value = 0;                                      // start stripe count
  // ref_time_ = utils::Timer::getTimeMicros();
}

StripeCounter FakeGpioCounter::getData(){
  // calculate if missed stripe here- so need to cross reference acceleration data
  if(timeCheck()){    // only do this if it's time to check

    data::Navigation nav   = data_.getNavigationData();
    // data::State state = data_.getStateMachineData().current_state;       // should I reference StateMachine for acc?
    uint32_t prev_count    = stripe_count_.count.value;
    uint32_t current_sector = stripe_count_.count.value*kStripeDistance;  // which stripe sector pod is in
    uint32_t next_sector = stripe_count_.count.value*kStripeDistance;   // distance of next stripe will hit
    
    if(nav.distance>current_sector&&nav.distance<next_sector){    // fix this data type for nav.distance!
    // if correct
      stripe_count_.count.value = prev_count;
      stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
      log_.INFO("fake_gpio_counter","correct count");
      // return stripe_count_;
    }
    else if(nav.distance<current_sector){       // counted too many stripes
      stripe_count_.count.value--;
      stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
      log_.INFO("fake_gpio_counter","incorrect count, substracting stripe");
      // return stripe_count_;
    }
    else if(nav.distance>next_sector){      // if missed extra_count number of stripes
      uint64_t extra_count = std::floor((nav.distance-current_sector)/kStripeDistance);
      stripe_count_.count.value += extra_count;
      stripe_count_.count.timestamp = utils::Timer::getTimeMicros();
      log_.INFO("fake_gpio_counter","incorrect count, %d stripes missed",extra_count);
      // return stripe_count_;
    }
      log_.INFO("fake_gpio_counter", "nav.distance=%f, new_count=%d, timestamp=%f", nav.distance, stripe_count_.count.value, stripe_count_.count.timestamp);

  }
  return stripe_count_;
}

bool FakeGpioCounter::getDistance(){ 
  data::Navigation nav = data_.getNavigationData();
  data::State state = data_.getStateMachineData().current_state;
  // uint32_t prev_count    = stripe_count_.count.value;

  StripeCounter current_data = getData();
  if(nav.acceleration<0 && state == data::State::kNominalBraking){      // TODO(Gregory): check state machine paths
    is_accelerating_ = false;
  } else if(nav.acceleration>0 || state == data::State::kAccelerating){
    is_accelerating_ = true;
  }
  if(timeout(current_data)){    // based on stripe_count_only 
    // need to stop
    log_.INFO("fake_gpio_counter", "Need to break...breaking now");
  } else{
    log_.INFO("fake_gpio_counter", "Keyence online...continue run");
  }
}

bool FakeGpioCounter::timeout(StripeCounter stripe_data){ // if no track left to safely break, based on stripe_counter_
  data::Navigation nav = data_.getNavigationData();
  uint32_t current_distance = stripe_data.count.value*kStripeDistance;

  if(kTrackDistance-current_distance < nav.braking_distance){   // have exceeded minimum braking distance
    log_.ERR("fake_gpio_counter", "Distance too short! We're gonna crash!");
    return true;
  }
  return false;
}

bool FakeGpioCounter::timeCheck(){    // used to see if it is time to check
  if(utils::Timer::getTimeMicros()-stripe_count_.count.timestamp>=check_time_){
    // if it is time check acc data to see if missed stripe
    return true;
  }
  return false;
}

}}
