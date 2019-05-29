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

#ifndef SENSORS_TEMP_MANAGER_HPP_
#define SENSORS_TEMP_MANAGER_HPP_

#include <cstdint>

#include "data/data.hpp"
#include "utils/system.hpp"

namespace hyped {

using utils::Logger;

namespace sensors {

class TempManager {
  // typedef data::DataPoint<array<TemperatureData, data::Sensors::kNumThermistors>>  DataArray;

 public:
  explicit TempManager(Logger& log);
  void runTemperature();
 private:
  bool temperatureInRange();

  Logger&                       log_;
  utils::System&                sys_;
  data::TemperatureData         pod_temp_;
  data::Data&                   data_;
  uint8_t                       analog_pins_[data::TemperatureData::kNumThermistors];
  Temperature*                  temp_[data::TemperatureData::kNumThermistors];
};

}}    // namespace hyped::sensors

#endif  // SENSORS_TEMP_MANAGER_HPP_
