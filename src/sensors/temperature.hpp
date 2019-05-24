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
#include "utils/io/i2c.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

namespace hyped {

using utils::io::I2C;
using utils::Logger;
using utils::concurrent::Thread;
using utils::System;

namespace sensors {

class Temperature {
 public:
  /**
   * @brief Construct a new Temperature object
   *
   * @param log from main thread, for debugging purposes
   * @param pin for specific ADC pin
   */
  Temperature(utils::Logger& log);
  ~Temperature() {}

  /**
   * @brief individual thread for Temperature
   *
   */
  void checkSensor();

  /**
   * @brief return temperature in degrees C
   * 
   * @return int 
   */
  int getTemperature();

 private:

  void configure();
  void writeByte(uint8_t write_reg, uint8_t write_data);
  void readByte(uint8_t read_reg, uint8_t *read_data);
  void readBytes(uint8_t read_reg, uint8_t *read_data, uint8_t length);
  int16_t averageData(int16_t data[5]);
  I2C& i2c_;
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
