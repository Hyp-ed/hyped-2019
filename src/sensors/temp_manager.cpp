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
    : log_(log),
      sys_(System::getSystem()),
      data_(Data::getInstance()),
      analog_pins_ {1}
{
  for (int i = 0; i < data::TemperatureData::kNumThermistors; i++) {    // creates new real objects
    temp_[i] = new Temperature(log, analog_pins_[i]);
  }
  log_.INFO("TEMP-MANAGER", "temp data has been initialised");
}

void TempManager::runTemperature()
{
  while (sys_.running_) {
    // int average = 0;
    for (int i = 0; i < data::TemperatureData::kNumThermistors; i++) {
      int value = temp_[i]->getAnalogRead().temp;
      log_.DBG1("TEMP-MANAGER", "Thermistor %d: %d", i, value);
    }


    // average = round(average/data::TemperatureData::kNumThermistors);
    // log_.DBG1("TEMP-MANAGER", "Average after rounding: %d", average);
    // pod_temp_.temp.value = average;
    // pod_temp_.temp.timestamp = utils::Timer::getTimeMicros();
    // log_.DBG1("TEMP-MANAGER", "pod_temp_: %d", pod_temp_.temp.value);

    // check ambient temperature
    // if (pod_temp_.module_status != data::ModuleStatus::kCriticalFailure) {
    //   if (!temperatureInRange()) {
    //     log_.ERR("TEMP-MANAGER", "temperature spike detected");
    //     pod_temp_.module_status = data::ModuleStatus::kCriticalFailure;
    //   }
    // }
    // data_.setTemperature(pod_temp_);
  }
}

int Temperature::averageData(int data[kAverageSet])
{
  double mean = 0;
  for (int i = 0; i < kAverageSet; i++) {
    mean += data[i];
  }
  mean = mean/kAverageSet;
  double sq_sum = 0;
  for (int i = 0; i < kAverageSet; i++) {
    sq_sum += pow((data[i]-mean), 2);
  }
  double st_dev = sqrt((sq_sum/kAverageSet));
  int final_sum = 0;
  int count = 0;
  for (int i = 0; i < kAverageSet; i++) {
    if (abs(data[i] - mean) < (st_dev*kAccuracyFactor)) {
      final_sum += data[i];
      count++;
    }
  }
  return round(final_sum/count);
}

bool TempManager::temperatureInRange()    // TODO(Anyone): add true temperature range
{
  auto& temperature = pod_temp_.temp;
  log_.DBG1("TEMP-MANAGER", "Temperature from data struct: %d", temperature);
  if (temperature < -10 || temperature > 90) {  // temperature in -10C to 50C
    log_.ERR("TEMP-MANAGER", "Temperature out of range: %d", temperature);
    return false;
  }
}

}}  // namespace hyped::sensors
