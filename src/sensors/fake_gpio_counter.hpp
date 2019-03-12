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

#ifndef SENSORS_FAKE_GPIO_COUNTER_HPP_
#define SENSORS_FAKE_GPIO_COUNTER_HPP_

#include <string>
#include <vector>

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"
#include "sensors/interface.hpp"            // GpioInterface

namespace hyped {

using utils::Logger;
using data::Data;

namespace sensors {

class FakeGpioCounter:public GpioInterface {
    public:
        FakeGpioCounter(Logger& log, bool miss_stripe);
        data::StripeCounter getStripeCounter() override;    // data from struct
        // struct StripeCounter {          // all raw data in form of this
        //     uint64_t timestamp;
        //     int value;
        // };
        StripeCounter getData();
    private:
        bool timeCheck();               // return if check_time_ exceeded
        bool timeout(StripeCounter stripe_data);      // if needs to break out  
        bool getDistance();             // return if OK or keyence_fail
        uint64_t getAccTime();
        Logger&     log_;
        Data&       data_;

        uint64_t              start_time_;      // just zero
        uint64_t              ref_time_;        // current time?
        uint64_t              check_time_;      // base this time off imuData
        uint64_t              brake_time_;
        StripeCounter         stripe_count_;
        // StripeCounter         prev_stripe_count_;
        bool                  miss_stripe_;
        bool                  is_accelerating_;
};

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


}}

#endif // SENSORS_FAKE_GPIO_COUNTER_HPP_