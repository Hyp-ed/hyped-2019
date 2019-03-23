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

#ifndef SENSORS_FAKE_GPIO_COUNTER_FILE_HPP_
#define SENSORS_FAKE_GPIO_COUNTER_COUNTER_FILE_HPP_

#include <string>
#include <vector>

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"
#include "sensors/fake_gpio_counter_interface.hpp"            // GpioInterface

namespace hyped {

using utils::Logger;
using data::Data;
using data::StripeCounter;

namespace sensors {

class FakeGpioCounter:public FakeGpioCounterInterface {
 public:

  // void FakeGpioCounterDynamic::readFromFile(std::vector<StripeCounter>& data)
  // {
  //   std::ifstream data_file ("fake_gpio_counter.txt");
  //   float count;
  //   float time;
  //   if (data_file.is_open()) {
  //     while (data_file >> count) {
  //       data_file >> time;
  //       StripeCounter this_line;
  //       this_line.count.value = count;
  //       this_line.count.timestamp = time;
  //       data.push_back(this_line);
  //     }
  //   }
  //   data_file.close();
  // }

  // void FakeGpioCounterDynamic::readData(std::vector<StripeCounter> data)
  // {
  //   stripe_count_.count.value = data.front().count.value;
  //   stripe_count_.count.value = data.front().count.timestamp;
  //   data.erase(data.begin());
  // }
  
private:
  
};

}}

#endif  // SENSORS_FAKE_GPIO_COUNTER_COUNTER_FILE_HPP_
