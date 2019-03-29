/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 * Main initialises sensor drivers. Main is not responsible for initialisation
 * of supporting io drivers (i2c, spi, can). This should be done by the sensor
 * drivers themselves.
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

#ifndef SENSORS_MAIN_HPP_
#define SENSORS_MAIN_HPP_

#include <cstdint>

#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "sensors/gpio_counter.hpp"
#include "sensors/interface.hpp"
#include "sensors/manager_interface.hpp"
#include "utils/system.hpp"

namespace hyped {

namespace sensors {

/**
 * @brief Initialise sensors, data instances to be pulled in managers, gpio threads declared in main
 *
 */
class Main: public Thread {
  public:
    Main(uint8_t id, Logger& log);
    void run() override;    // from thread

  private:
    data::Data&     data_;
    utils::System& sys_;

    // master data structures
    data::Batteries batteries_;
    data::StripeCounter stripe_counter_;

    GpioCounter*                           keyence_l_;
    GpioCounter*                           keyence_r_;
    std::unique_ptr<ImuManagerInterface>   imu_manager_;
    std::unique_ptr<ManagerInterface>      battery_manager_;

    bool sensor_init_;
    bool battery_init_;
};

}}

#endif  // SENSORS_MAIN_HPP_
