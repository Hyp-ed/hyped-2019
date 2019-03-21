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

#include "sensors/main.hpp"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#include "data/data.hpp"
#include "sensors/imu_manager.hpp"
#include "sensors/bms_manager.hpp"
#include "sensors/gpio_counter.hpp"

namespace hyped {

using data::StripeCounter;
using hyped::utils::concurrent::Thread;

namespace sensors {

 Main::Main(uint8_t id, Logger& log)
  : Thread(id, log),
    data_(data::Data::getInstance()),
    sys_(utils::System::getSystem()),
    imu_manager_(new ImuManager(log, &sensors_.imu)),
    battery_manager_(new BmsManager(log,
                                        &batteries_.low_power_batteries,
                                        &batteries_.high_power_batteries)),
    sensor_init_(false),
    battery_init_(false)
  {
    // Pins for keyence GPIO_36 L and GPIO_33 R
    GpioCounter* temp;
    temp = new GpioCounter(36);
    temp->start();
    keyence_l_ = temp;

    temp = new GpioCounter(33);
    temp->start();
    keyence_r_ = temp;
}

void Main::run()
{

}

}}
