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

/**
 * Testing Keyence stripe detection: 
    * logger
    * data
    * start_time, max_time
    * stripe count (same as data?)
    * missed stripe? (bool)
    * accelerating? (bool)
 * 
 * Check if missed stripe:
 * compare time with fake_acceleration data- see if accelerating or decelerating
 * switch states of of state machine in consideration of acceleration
 * increase stripe count of missed, log sensor failure
 * 
 * Reach end of run if:
 * max speed based off acceleration/time
 * max time based acceleration/velocity and stripe count
 * max distance based off stripe count
 * --> case: end gracefully and emergency state
 * 
 * FUNCTIONS (with subfunctions?)
 * 
 * Timer count function to:
 *  calculate time left in run
 *  bool if need to break
 * 
 * getData function with:
 *  isAccelerating?
 *  missedStripe?
 * 
 * getDistance function:
 *  calculate overall distance based solely on stripe count
 *  cross reference imu data (.5at^2)
 *  override if missed stripe
 *  return bool to end run
 * 
 */

/**
 * StripeCounter: DataPoint<uint32_t>, which is a template of 
 * typename 'T' of timestamp and value of type 'T' (2param)
 * 
 * Data needed: 
 * Navigation data (distance, velocity, acc, embrake distance, braking distance)- typedef float
 * State Machine data (critical failure, current state-enum)
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

constexpr uint64_t kBrakeTime = 10000000;     // calculate this constant from FDP

namespace hyped {

namespace sensors {

FakeGpioCounter::FakeGpioCounter(Logger& log, bool miss_stripe)
    : log_(log),
      start_time_(0),                // just zero
      ref_time_(start_time_),        // current time?
      check_time_(5000000),                 // base this time off imuData
      brake_time_(kBrakeTime),                   
      // stripe_count_(0);
      miss_stripe_(miss_stripe),
      is_accelerating_(false)
{
  stripe_count_.timestamp = utils::Timer::getTimeMicros();      // start time
  stripe_count_.value = 0;                                      // start stripe count
}





}}
