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

#include <stdio.h>
#include "utils/concurrent/thread.hpp"
#include "utils/io/adc.hpp"
#include "utils/timer.hpp"
#include "sensors/temperature.hpp"

namespace hyped {

using data::Data;
using data::TemperatureData;
using utils::concurrent::Thread;
using utils::io::ADC;
using hyped::utils::Logger;

namespace sensors {

Temperature::Temperature(utils::Logger& log, int pin)
     : pin_(pin),
       sys_(utils::System::getSystem()),
       log_(log)
{
  
}

void Temperature::run()
{
  ADC thepin(pin_);
  temp_.temp.value = 0;
  temp_.temp.timestamp = utils::Timer::getTimeMicros();

  while (sys_.running_) {
    int raw_value = thepin.read();
    // log_.DBG1("Temperature", "Raw Data: %d", raw_value);
    temp_.temp.value = scaleData(raw_value);
    temp_.temp.timestamp = utils::Timer::getTimeMicros();
    temp_.operational = true;
  }
}

// TODO(Anyone): scale data correctly
int Temperature::scaleData(int raw_value)
{
  // return ((raw_value*(180/255)) - 55);
  return raw_value;
}

TemperatureData Temperature::getAnalogRead()
{
  return temp_;
}
}}  // namespace hyped::sensors
