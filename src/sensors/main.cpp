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
#include "sensors/imu_manager.hpp"
#include "sensors/bms_manager.hpp"
#include "sensors/gpio_counter.hpp"
#include "sensors/fake_gpio_counter.hpp"


namespace hyped {

using hyped::utils::concurrent::Thread;
using utils::System;
using data::Data;
using data::Sensors;
using data::StripeCounter;
using data::SensorCalibration;


namespace sensors {

Main::Main(uint8_t id, utils::Logger& log)
  : Thread(id, log),
    data_(data::Data::getInstance()),
    sys_(utils::System::getSystem()),
    log_(log),
    pins_ {36, 33},
    imu_manager_(new ImuManager(log)),
    battery_manager_(new BmsManager(log,
                                        &batteries_.low_power_batteries,
                                        &batteries_.high_power_batteries))
{
  if (!sys_.fake_keyence) {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      GpioCounter* keyence = new GpioCounter(log_, pins_[i]);
      keyence->start();
      keyences_[i] = keyence;
    }
  } else {
    for (int i =0; i < data::Sensors::kNumKeyence; i++) {
      keyences_[i] = new FakeGpioCounter(log_, false, false);
    }
  }
}

bool Main::keyencesUpdated()
{
  for (int i = 0; i < data::Sensors::kNumKeyence; i ++) {
    if (prev_keyence_stripe_count_arr_[i].count.value != keyence_stripe_counter_arr_[i].count.value)
      return true;
  }
  return false;
}

void Main::run()
{
// start all managers
  imu_manager_->start();
  battery_manager_->start();

  // Initalise the keyence arrays
  keyence_stripe_counter_arr_    = data_.getSensorsData().keyence_stripe_counter;
  prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;

  while (sys_.running_) {
    // We need to read the gpio counters and write to the data structure
    // If previous is not equal to the new data then update
    if (keyencesUpdated()) {
      // Update data structure, make prev reading same as this reading
      data_.setSensorsKeyenceData(keyence_stripe_counter_arr_);
      prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;
    }
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      prev_keyence_stripe_count_arr_[i] = keyences_[i]->getStripeCounter();
    }
    Thread::sleep(10);  // Sleep for 10ms
  }

  imu_manager_->join();
  battery_manager_->join();
}
}}
