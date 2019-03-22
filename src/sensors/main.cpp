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
    imu_manager_(new ImuManager(log, &sensors_.imu)),
    battery_manager_(new BmsManager(log,
                                        &batteries_.low_power_batteries,
                                        &batteries_.high_power_batteries)),
    sensor_init_(false),
    battery_init_(false)
  {
    // Pins for keyence GPIO_36 L and GPIO_33 R
    GpioCounter* temp;
    temp = new GpioCounter(36);
    temp->start();
    keyence_l_ = temp;

    temp = new GpioCounter(33);
    temp->start();
    keyence_r_ = temp;
}

void Main::run()
{
// start all managers
  imu_manager_->start();
  battery_manager_->start();

  // init loop
  while (!sensor_init_) {
    if (imu_manager_->updated()) {
      data_.setSensorsData(sensors_);

      // Get calibration data
      SensorCalibration sensor_calibration_data;      // TODO(Greg): done in imu_manager.cpp
      sensor_calibration_data.imu_variance  = imu_manager_->getCalibrationData();
      data_.setCalibrationData(sensor_calibration_data);
      sensor_init_ = true;

      break;
    }
    yield();
  }
  log_.INFO("SENSORS", "sensors data has been initialised");
  while (!battery_init_) {
    if (battery_manager_->updated()) {
      data_.setBatteryData(batteries_);     // TODO(Greg): config data type for param
      battery_init_ = true;
      break;
    }
    yield();
  }
  log_.INFO("SENSORS", "batteries data has been initialised");


  // Need any of the following??
  if (sensor_init_) sensors_.module_status = data::ModuleStatus::kInit;
  if (battery_init_) batteries_.module_status = data::ModuleStatus::kInit;

  // work loop
  while (sys_.running_) {
    // Write sensor data to data structure only when all the imu or proxi values are different
    if (imu_manager_->updated()) {
      sensors_.keyence_stripe_counter[0] = keyence_l_->getStripeCounter();
      sensors_.keyence_stripe_counter[1] = keyence_r_->getStripeCounter();
      data_.setSensorsData(sensors_);
      // Update manager timestamp with a function
      imu_manager_->resetTimestamp();
    }
    // Update battery data only when there is some change
    if (battery_manager_->updated()) {
      battery_manager_->resetTimestamp();

      // check health of batteries
      if (batteries_.module_status != data::ModuleStatus::kCriticalFailure) {
        if (!batteriesInRange()) {
          log_.ERR("SENSORS", "battery failure detected");
          batteries_.module_status = data::ModuleStatus::kCriticalFailure;
        }
      }
      // publish the new data
      data_.setBatteryData(batteries_);
    }
    yield();
  }
}

}}
