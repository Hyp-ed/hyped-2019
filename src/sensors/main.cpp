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

#include "sensors/main.hpp"

#include "data/data.hpp"
#include "sensors/imu_manager.hpp"
#include "sensors/bms_manager.hpp"
#include "sensors/gpio_counter.hpp"

constexpr int kKeyencePinLeft = 36;
constexpr int kKeyencePinRight = 33;

namespace hyped {

using hyped::utils::concurrent::Thread;
using utils::System;
using data::Data;
using data::Sensors;
using data::Batteries;
using data::StripeCounter;
using data::SensorCalibration;


namespace sensors {

Main::Main(uint8_t id, Logger& log)
  : Thread(id, log),
    data_(data::Data::getInstance()),
    sys_(utils::System::getSystem()),
    imu_manager_(new ImuManager(log)),
    battery_manager_(new BmsManager(log)),
    keyence_l_(new GpioCounter(kKeyencePinLeft)),
    keyence_r_(new GpioCounter(kKeyencePinRight))
  {}

void Main::run()
{
// start all managers
  imu_manager_->start();
  battery_manager_->start();

  // Pins for keyence GPIO_36 L and GPIO_33 R
  keyence_l_->start();
  keyence_r_->start();

  // Declare arrays
  array<StripeCounter, data::Sensors::kNumKeyence> keyence_stripe_counter;
  array<StripeCounter, data::Sensors::kNumKeyence> prev_keyence_stripe_count;

  // Initalise the arrays     // TODO(Jack): can you do this? throws compilation error
  keyence_stripe_counter = data_.getSensorsData().keyence_stripe_counter;
  prev_keyence_stripe_count = keyence_stripe_counter;

  while (sys_.running_) {
    // We need to read the gpio counters and write to the data structure
    // If previous is not equal to the new data then update
    if (prev_keyence_stripe_count[0].count.value != keyence_stripe_counter[0].count.value ||
        prev_keyence_stripe_count[1].count.value != keyence_stripe_counter[1].count.value ) {
      // Update data structure, make prev reading same as this reading
      data_.setSensorsKeyenceData(keyence_stripe_counter);
      prev_keyence_stripe_count = keyence_stripe_counter;
    }
    keyence_stripe_counter[0] = keyence_l_->getStripeCounter();
    keyence_stripe_counter[1] = keyence_r_->getStripeCounter();
    Thread::sleep(10);  // Sleep for 10ms
  }

  imu_manager_->join();
  battery_manager_->join();
  keyence_l_->join();
  keyence_r_->join();
}


}}
