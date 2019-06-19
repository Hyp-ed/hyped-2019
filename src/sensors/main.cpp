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
#include "sensors/temperature.hpp"
#include "sensors/gpio_counter.hpp"
#include "sensors/fake_gpio_counter.hpp"
#include "sensors/fake_temperature.hpp"


constexpr int kKeyencePinLeft = 72;
constexpr int kKeyencePinRight = 74;
constexpr int kThermistorPin = 3;

namespace hyped {

using hyped::utils::concurrent::Thread;
using utils::System;
using data::Data;
using data::Sensors;
using data::StripeCounter;

namespace sensors {

Main::Main(uint8_t id, utils::Logger& log)
  : Thread(id, log),
    data_(data::Data::getInstance()),
    sys_(utils::System::getSystem()),
    log_(log),
    pins_ {kKeyencePinLeft, kKeyencePinRight},
    imu_manager_(new ImuManager(log)),
    battery_manager_(new BmsManager(log))
{
  if (!sys_.fake_keyence) {
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      GpioCounter* keyence = new GpioCounter(log_, pins_[i]);
      keyence->start();
      keyences_[i] = keyence;
    }
  } else {
    for (int i =0; i < data::Sensors::kNumKeyence; i++) {
      keyences_[i] = new FakeGpioCounter(log_, false, false, "data/in/gpio_counter_normal_run.txt");
    }
  }
  if (!sys_.fake_temperature) {
    temperature_ = new Temperature(log_, kThermistorPin);
  } else {
    temperature_ = new FakeTemperature(log_, false);
  }
  // kInit for SM transition
  sensors_ = data_.getSensorsData();
  sensors_.module_status = data::ModuleStatus::kInit;
  data_.setSensorsData(sensors_);
  log_.INFO("Sensors", "Sensors have been initialised");
}

bool Main::keyencesUpdated()
{
  for (int i = 0; i < data::Sensors::kNumKeyence; i ++) {
    if (prev_keyence_stripe_count_arr_[i].count.value != keyence_stripe_counter_arr_[i].count.value)
      return true;
  }
  return false;
}

bool Main::temperatureInRange()    // TODO(anyone): add true temperature range
{
  auto temperature = data_.getTemperature();
  log_.DBG1("Sensors", "Temperature from data struct: %d", temperature);
  if (temperature < -10 || temperature > 60) {  // temperature in -10C to 60C
    log_.ERR("Sensors", "Temperature out of range: %d", temperature);
    return false;
  }
  return true;
}

void Main::checkTemperature()
{
  temperature_->run();               // not a thread
  data_.setTemperature(temperature_->getData());
  if (!temperatureInRange()) {
    log_.ERR("Sensors", "Temperature out of range: Critical Failure!");
    auto status = data_.getSensorsData();
    status.module_status = data::ModuleStatus::kCriticalFailure;
    data_.setSensorsData(status);    // TemperatureData not part of Sensors
  }
}

void Main::run()
{
// start all managers
  imu_manager_->start();
  battery_manager_->start();

  // Initalise the keyence arrays
  keyence_stripe_counter_arr_    = data_.getSensorsData().keyence_stripe_counter;
  prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;

  int temp_count = 0;
  while (sys_.running_) {
    // We need to read the gpio counters and write to the data structure
    // If previous is not equal to the new data then update
    if (keyencesUpdated()) {
      // Update data structure, make prev reading same as this reading
      data_.setSensorsKeyenceData(keyence_stripe_counter_arr_);
      prev_keyence_stripe_count_arr_ = keyence_stripe_counter_arr_;
    }
    for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
      keyence_stripe_counter_arr_[i] = keyences_[i]->getStripeCounter();
    }
    Thread::sleep(10);  // Sleep for 10ms
    temp_count++;
    if (temp_count % 20 == 0) {       // check every 20 cycles of main
      checkTemperature();
    }
  }

  imu_manager_->join();
  battery_manager_->join();
}
}}
