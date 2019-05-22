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

#include "sensors/temp_manager.hpp"
#include "sensors/temperature.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"

namespace hyped {

using data::Data;
using data::Sensors;
using utils::System;

namespace sensors {
TempManager::TempManager(Logger& log)
    : TempManagerInterface(log),
      sys_(System::getSystem()),
      data_(Data::getInstance()),
      analog_pins_ {0}
{
  for (int i = 0; i < data::Sensors::kNumThermistors; i++) {    // creates new real objects
    Temperature* temp = new Temperature(log, analog_pins_[i]);
    temp->start();
    temp_[i] = temp;
  }
  log_.INFO("TEMP-MANAGER", "temp data has been initialised");
}

void TempManager::run()
{
  while(sys_.running_) {
    int average = 0;
    for (int i = 0; i < data::Sensors::kNumThermistors; i++) {
      average += temp_[i]->getAnalogRead().temp.value;      // TODO(anyone): average correctly
      log_.DBG1("TEMP-MANAGER", "Sum: %d", average);
    }
    average = round(average/data::Sensors::kNumThermistors);
    log_.DBG1("TEMP-MANAGER", "Average after rounding: %d", average);
    pod_temp_.temp.value = average;
    pod_temp_.temp.timestamp = utils::Timer::getTimeMicros();
    log_.DBG1("TEMP-MANAGER", "pod_temp_: %d", pod_temp_.temp.value);

    // check ambient temperature
    if (pod_temp_.module_status != data::ModuleStatus::kCriticalFailure) {
      if (!temperatureInRange()) {
        log_.ERR("TEMP-MANAGER", "temperature spike detected");
        pod_temp_.module_status = data::ModuleStatus::kCriticalFailure;
      }
    }
    data_.setTemperature(pod_temp_);
  }
}

bool TempManager::temperatureInRange()    // TODO(Anyone): add true temperature range
{
  auto& temperature = pod_temp_.temp;
  log_.DBG1("TEMP-MANAGER", "Temperature from data struct: %d", temperature);
  if (temperature.value < -10 || temperature.value > 90) {  // temperature in -10C to 50C
    log_.ERR("TEMP-MANAGER", "Temperature out of range: %d", temperature.value);
    return false;
  }
}

}}  // namespace hyped::sensors
