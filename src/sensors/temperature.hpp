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

constexpr int kAverageSet = 5;
constexpr int kAccuracyFactor = 2;      // change if needed

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
   */
  explicit Temperature(utils::Logger& log);
  ~Temperature() {}

  /**
   * @brief Called by sensors main, runs kAverageSet times to account for noise
   *        sets average to data struct
   */
  void checkSensor();

  /**
   * @brief return temperature in degrees C
   *
   * @return int
   */
  int getTemperature();

 private:
  /**
   * @brief called at initialisation, software reset and 16-bit resolution
   *        active high and interrupt mode for CT and INT pins, left default continuous mode
   *
   */
  void configure();

  /**
   * @brief write one byte to register via i2c
   *
   * @param write_reg register to write data to
   * @param write_data one byte of data to write
   */
  void writeByte(uint8_t write_reg, uint8_t write_data);

  /**
   * @brief read one byte from register using i2c_addr to first write then read
   *
   * @param read_reg address register where reading data from
   * @param read_data pointer to set up adddress of register where reading from
   */
  void readByte(uint8_t read_reg, uint8_t *read_data);

  /**
   * @brief readByte config for byte length
   *
   * @param read_reg
   * @param read_data
   * @param length number of bytes
   */
  void readBytes(uint8_t read_reg, uint8_t *read_data, uint8_t length);

  /**
   * @brief takes mean and standard deviation of array size kAverageSet
   *        omits data outside (kAccuracyFactor x st_dev)
   *
   * @param data array of kAverageSet
   * @return int average
   */
  int averageData(int data[kAverageSet]);

  I2C& i2c_;
  System& sys_;
  utils::Logger& log_;

  /**
   * @brief DataPoint of type int
   */
  data::TemperatureData temp_;
};

}}

#endif  // SENSORS_TEMPERATURE_HPP_
