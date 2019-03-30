/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 19/06/18
 * Description: IMU manager for getting IMU data from around the pod
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


#include "sensors/imu_manager.hpp"

#include "sensors/imu.hpp"
#include "sensors/fake_imu.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"

namespace hyped {

using data::Data;
using data::Sensors;
using utils::System;
using data::NavigationVector;

namespace sensors {
ImuManager::ImuManager(Logger& log)
    : ImuManagerInterface(log),
      sys_(System::getSystem()),
      data_(Data::getInstance()),
      chip_select_ {49, 117, 125, 123, 111, 112, 110, 20}
{
  old_timestamp_ = utils::Timer::getTimeMicros();
  utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k1MHz);

  if (!sys_.fake_imu) {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {   // creates new real IMU objects
      imu_[i] = new Imu(log, chip_select_[i], 0x08);
    }
  } else {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {
      imu_[i] = new FakeImuFromFile(log,
                                    "data/in/acc_state.txt",
                                    "data/in/decel_state.txt",
                                    "data/in/decel_state.txt");
    }
  }
  utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k20MHz);
  log_.INFO("IMU-MANAGER", "imu data has been initialised");
}

void ImuManager::run()
{
  // collect real data while system is running
  while (sys_.running_) {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {
      imu_[i]->getData(&(sensors_imu_.value[i]));
    }
    resetTimestamp();
    sensors_imu_.timestamp = utils::Timer::getTimeMicros();
    data_.setSensorsImuData(sensors_imu_);
  }
}

bool ImuManager::updated()
{
  if (old_timestamp_ != sensors_imu_.timestamp) {
    return true;
  }
  return false;
}

void ImuManager::resetTimestamp()
{
  old_timestamp_ = sensors_imu_.timestamp;
}
}}  // namespace hyped::sensors
