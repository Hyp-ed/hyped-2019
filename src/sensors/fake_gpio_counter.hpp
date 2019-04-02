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

#ifndef SENSORS_FAKE_GPIO_COUNTER_HPP_
#define SENSORS_FAKE_GPIO_COUNTER_HPP_

#include <string>
#include <vector>
#include "data/data.hpp"
#include "sensors/interface.hpp"
#include "utils/logger.hpp"

namespace hyped {

using utils::Logger;
using data::Data;
using data::StripeCounter;

namespace sensors {

class FakeGpioCounter : public GpioInterface {
 public:
  /**
   * @brief Construct a new Fake Gpio Counter dynamic object
   *
   * @param log
   * @param miss_stripe
   * @param double_stripe
   */
  FakeGpioCounter(utils::Logger& log, bool miss_stripe, bool double_stripe);

  /**
   * @brief Construct a new Fake Gpio Counter object from file
   *
   * @param log
   * @param miss_stripe
   * @param double_stripe
   * @param file_path
   */
  FakeGpioCounter(utils::Logger& log, bool miss_stripe, bool double_stripe, std::string file_path);

  /**
   * @brief Returns the current count of stripes
   *
   * @return data::StripeCounter stripe count and timestamp (microseconds)
   */
  StripeCounter getStripeCounter() override;

  /**
   * @brief call this function when you want to read first entry of stripe_data_ from the main
   *
   */
  void readData();

 private:
  bool timeCheck();         // return if check_time_ exceeded

   /**
   * @brief based on flags from getData(), overrides stripe_count_ if not correct
   * continues after first 5 seconds of run, call this after getData()
   */
  void checkData();

  bool timeout(StripeCounter stripe_data);    // if needs to break out
  void readFromFile(std::vector<StripeCounter>& data);
  Logger& log_;
  Data& data_;

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
  uint64_t check_time_ = 358588;

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

  std::string file_path_;

  /**
   * @brief vector of StripeCounter data read from file
   *
   */
  std::vector<StripeCounter> stripe_data_;
};

}}  // namespace hyped::sensors


#endif  // SENSORS_FAKE_GPIO_COUNTER_HPP_
