/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 1/6/19
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
// using data::Sensors;
using utils::System;

namespace sensors {
TempManager::TempManager(Logger& log)
    : log_(log),
      sys_(System::getSystem()),
      data_(Data::getInstance()),
      analog_pins_ {0, 1, 2, 3}
{
  for (int i = 0; i < data::TemperatureData::kNumThermistors; i++) {    // creates new real objects
    temp_[i] = new Temperature(log, analog_pins_[i]);
  }
  log_.INFO("TEMP-MANAGER", "temp data has been initialised");
}

void TempManager::runTemperature()
{
  for (int i = 0; i < data::TemperatureData::kNumThermistors; i++) {
    temp_data_[i].temp = temp_[i]->getAnalogRead().temp;
    log_.DBG1("TEMP-MANAGER", "Thermistor %d: %d", i, temp_data_[i]);
  }
  pod_temp_ = averageData();
}

int TempManager::averageData()
{
  double mean = 0;
  for (int i = 0; i < kAverageSet; i++) {
    mean += temp_data_[i].temp;
  }
  mean = mean/kAverageSet;
  double sq_sum = 0;
  for (int i = 0; i < kAverageSet; i++) {
    sq_sum += pow((temp_data_[i].temp-mean), 2);
  }
  double st_dev = sqrt((sq_sum/kAverageSet));
  int final_sum = 0;
  int count = 0;
  for (int i = 0; i < kAverageSet; i++) {
    if (abs(temp_data_[i].temp - mean) < (st_dev*kAccuracyFactor)) {
      final_sum += temp_data_[i].temp;
      count++;
    } else {
      temp_data_[i].operational = false;          // invalid output turns inoperational
    }
  }
  return round(final_sum/count);      // remove outliers
}

int TempManager::getPodTemp()
{
  return pod_temp_;
}

}}  // namespace hyped::sensors
