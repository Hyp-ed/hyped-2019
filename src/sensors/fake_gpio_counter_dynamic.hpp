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

#ifndef SENSORS_FAKE_GPIO_COUNTER_DYNAMIC_HPP_
#define SENSORS_FAKE_GPIO_COUNTER_DYNAMIC_HPP_

#include <string>
#include <vector>

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"
#include "sensors/fake_gpio_counter_interface.hpp"

namespace hyped {

using utils::Logger;
using data::Data;
using data::StripeCounter;
using sensors::FakeGpioCounterInterface;

namespace sensors {

class FakeGpioCounterDynamic:public FakeGpioCounterInterface {
 public:
  /**
  * @brief Construct a new Fake Gpio Counter object
  * 
  * @param log 
  * @param miss_stripe if missed single stripe
  * @param double_stripe_ if counted one extra stripe
  */
  FakeGpioCounterDynamic(Logger& log, bool miss_stripe, bool double_stripe_);

  StripeCounter getStripeCounter() override;

  StripeCounter getData() override;

  void checkData() override;

  void checkData() override;

  bool timeCheck() override;

  bool timeout(StripeCounter stripe_data) override;

  
private:
  Logger&     log_;
  Data&       data_;
};

}}

#endif  // SENSORS_FAKE_GPIO_COUNTER_DYNAMIC_HPP_
