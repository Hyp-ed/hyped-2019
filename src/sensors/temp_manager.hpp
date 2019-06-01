/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 1/6/19
 * Description: Handles readings from all thermistors on PCB, averages readings and
 *    sets faulty sensors offline, used in sensors main for cleaner implementation
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

#ifndef SENSORS_TEMP_MANAGER_HPP_
#define SENSORS_TEMP_MANAGER_HPP_

#include <cstdint>

#include "data/data.hpp"
#include "utils/system.hpp"
#include "temperature.hpp"

constexpr int kAverageSet = 5;
constexpr int kAccuracyFactor = 2;

namespace hyped {

using utils::Logger;

namespace sensors {

class TempManager {
 public:
  explicit TempManager(Logger& log);

  /**
   * @brief get temperature from each thermistor and average data, set to pod_temp_
   *
   */
  void runTemperature();
  /**
   * @brief
   *
   * @return int to be set to data struct in main
   */
  int getPodTemp();

 private:
  /**
   * @brief take standard deviation, remove values outside kAccuracyFactorx the stdev
   * need to handle type TemperatureData to set thermistor sensor isOnline = false if faulty
   *
   * @return int to be set as pod_temp_
   */
  int averageData();

  Logger&                       log_;
  utils::System&                sys_;

  /**
   * @brief int representation of data, to set to data struct
   */
  int                           pod_temp_;
  data::Data&                   data_;
  uint8_t                       analog_pins_[data::TemperatureData::kNumThermistors];

  /**
   * @brief points to Temperature objects for each thermistor
   */
  Temperature*                  temp_[data::TemperatureData::kNumThermistors];

  /**
   * @brief hold TemperatureData for each Temperature object
   */
  data::TemperatureData         temp_data_[data::TemperatureData::kNumThermistors];
};

}}    // namespace hyped::sensors

#endif  // SENSORS_TEMP_MANAGER_HPP_
