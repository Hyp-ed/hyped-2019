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
#include "utils/io/i2c.hpp"
#include "utils/timer.hpp"
#include "sensors/temperature.hpp"

namespace hyped {

using data::Data;
using data::TemperatureData;
using utils::concurrent::Thread;
using utils::io::I2C;
using hyped::utils::Logger;

namespace sensors {

Temperature::Temperature(utils::Logger& log)
     : sys_(utils::System::getSystem()),
       log_(log),
       i2c_(I2C::getInstance())
{
  // i2c_.write(...)
}

void Temperature::checkSensor()
{
  // temp_.temp = 0;
  // uint16_t raw_value = thepin.read();
  // log_.DBG1("Temperature", "Raw Data: %d", raw_value);
  // temp_.temp = scaleData(raw_value);
  // log_.DBG1("Temperature", "Scaled Data: %d", raw_value);
  // temp_.operational = true;
}


int Temperature::getTemperature() {
  return temp_.temp;
}

}}  // namespace hyped::sensors
