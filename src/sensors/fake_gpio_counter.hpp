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

#include <string>
#include <vector>

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"
#include "sensors/interface.hpp"            // GpioInterface

namespace hyped {

using utils::Logger;
// using data::Data;

namespace sensors {

class FakeGpioCounter:public SensorInterface {      // change to gpiointerface, but not included yet
    public:
        FakeGpioCounter(Logger& log, bool miss_stripe);
        // data::StripeCounter getStripeCounter() override;    // data from struct
        struct StripeCounter {          // all raw data in form of this
            uint64_t timestamp;
            int value;
        };
        StripeCounter getData();
    private:
        bool timeout();                 // return if check_time_ exceeded
        bool getDistance();             // return if OK or keyence_fail
        Logger&     log_;

        uint64_t              start_time_;      // just zero
        uint64_t              ref_time_;        // current time?
        uint64_t              check_time_;      // base this time off imuData
        uint64_t              brake_time_;
        StripeCounter         stripe_count_;
        bool                  miss_stripe_;
        bool                  is_accelerating_;
};



}}
