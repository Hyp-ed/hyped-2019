/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2018 HYPED
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

#ifndef SENSORS_FAKE_GPIO_COUNTER_INTERFACE_HPP_
#define SENSORS_FAKE_GPIO_COUNTER_INTERFACE_HPP_

#include <string>
#include "data/data.hpp"
#include "sensors/interface.hpp"

namespace hyped {

using data::StripeCounter;

namespace sensors {

class FakeGpioCounterInterface:public GpioInterface {
 public:

  /**
   * @brief from data.hpp
   * 
   * @return data::StripeCounter stripe count and timestamp (microseconds)
   */
  StripeCounter getStripeCounter() override;

  /**
   * @brief compares navigation data and sets miss_stripe_, double_stripe_ true 
   * if stripe_count_ does not match the data
   * 
   * @return StripeCounter 
   */
  virtual StripeCounter getData();

  /**
   * @brief based on flags from getData(), overrides stripe_count_ if not correct
   * continues after first 5 seconds of run 
   */
  virtual void checkData(); 

  virtual bool timeCheck();         // return if check_time_ exceeded

  virtual bool timeout(StripeCounter stripe_data);    // if needs to break out

 protected:
  /**
   * @brief check if 5 seconds have passed to start comparing navigation data with stripe counter
   * 
   */
  uint64_t start_time_;

  /**
   * @brief minimum time between stripes ().358588 seconds, max speeed 85 m/s) 
   * make sure not to miss two stripes in a row
   * 
   */
  uint64_t check_time_;

  /**
   * @brief current stripe data
   * 
   */
  StripeCounter stripe_count_;

    /**
   * @brief if missed single stripe, set true if does not match navigation data
   * 
   */
  bool miss_stripe_;

    /**
   * @brief if counted extra stripe, set true if does not match navigation data
   * 
   */
  bool double_stripe_;

  /**
   * @brief timestamp at beginning of run, used to get start time within getData()
   * 
   */
  bool init_;
};

}}  // namespace hyped::sensors


#endif  // SENSORS_FAKE_GPIO_COUNTER_INTERFACE_HPP_
