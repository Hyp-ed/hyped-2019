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

#include <stdio.h>
#include "utils/io/adc.hpp"
#include "sensors/temperature.hpp"

namespace hyped {

using data::Data;
using data::TemperatureData;
using utils::io::ADC;
using hyped::utils::Logger;

namespace sensors {

Temperature::Temperature(utils::Logger& log, int pin)
     : pin_(pin),
       log_(log)
{}

void Temperature::checkSensor()
{
  ADC thepin(pin_);
  temp_.temp = 0;
  uint16_t raw_value = thepin.read();
  log_.DBG1("Temperature", "Raw Data: %d", raw_value);
  temp_.temp = scaleData(raw_value);
  log_.DBG1("Temperature", "Scaled Data: %d", temp_.temp);
  temp_.operational = true;
}

// TODO(Anyone): scale data correctly for TMP35
int Temperature::scaleData(uint16_t raw_value)
{
  // convert to C temperature
  double temp = static_cast<double>(raw_value) / 4095;
  temp = (temp*165) - 40;
  return static_cast<int>(temp);
}

int Temperature::getAnalogRead()
{
  return temp_.temp;
}
}}  // namespace hyped::sensors
