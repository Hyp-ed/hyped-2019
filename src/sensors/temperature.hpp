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

#ifndef SENSORS_TEMPERATURE_HPP_
#define SENSORS_TEMPERATURE_HPP_

#include <cstdint>

#include "data/data.hpp"
#include "sensors/interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

namespace hyped {

using utils::Logger;
using utils::concurrent::Thread;
using utils::System;

namespace sensors {

class Temperature: public AdcInterface, public Thread {
 public:
  /**
   * @brief Construct a new Temperature object
   *
   * @param log from main thread, for debugging purposes
   * @param pin for specific ADC pin
   */
  Temperature(utils::Logger& log, int pin);
  ~Temperature() {}

  /**
   * @brief returns TemperatreData object, from interface
   *
   * @return data::TemperatureData
   */
  data::TemperatureData getAnalogRead() override;

  /**
   * @brief individual thread for Temperature
   *
   */
  void run() override;

 private:
  /**
   * @brief scale raw digital data to output in degrees C
   *
   * @param voltage
   * @return int
   */
  int scaleData(int voltage);
  int pin_;
  System& sys_;
  utils::Logger& log_;

  /**
   * @brief DataPoint of type int
   *
   */
  data::TemperatureData temp_;
};

}}

#endif  // SENSORS_TEMPERATURE_HPP_
