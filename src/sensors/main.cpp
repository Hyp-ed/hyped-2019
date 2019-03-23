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
    // imu_manager_(new ImuManager(log, &sensors_.imu)),
    battery_manager_(new BmsManager(log,
                                        &batteries_.low_power_batteries,
                                        &batteries_.high_power_batteries))
  {}

void Main::run()
{
// start all managers
  imu_manager_->start();
  battery_manager_->start();

  // Pins for keyence GPIO_36 L and GPIO_33 R
  GpioCounter* temp;
  temp = new GpioCounter(36);
  temp->start();
  keyence_l_ = temp;

  temp = new GpioCounter(33);
  temp->start();
  keyence_r_ = temp;

  // data_.setSensorsData(sensors_);

  // TODO(Greg): Need any of the following??
  
  // sensors_.module_status = data::ModuleStatus::kInit;
  // batteries_.module_status = data::ModuleStatus::kInit;

  // // work loop
  // while (sys_.running_) {
  //   // Write sensor data to data structure only when all the imu values are different
  //   if (imu_manager_->updated()) {
  //     // Update manager timestamp with a function
  //     imu_manager_->resetTimestamp();       // TODO(Greg): Where does this go?
  //   }
  // }
}


}}
