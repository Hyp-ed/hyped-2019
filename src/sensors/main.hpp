/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 * Main initialises and manages sensor drivers. Main is not responsible for initialisation
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
    Main(uint8_t id, utils::Logger& log);
    void run() override;    // from thread

  private:
    bool keyencesUpdated();

    data::Data&     data_;
    utils::System&  sys_;
    utils::Logger&  log_;

    // master data structures
    data::Sensors   sensors_;
    data::Batteries batteries_;
    data::StripeCounter stripe_counter_;

    uint8_t                                pins_[data::Sensors::kNumImus];
    GpioInterface*                         keyences_[data::Sensors::kNumKeyence];  // 0 L and 1 R
    std::unique_ptr<ImuManagerInterface>   imu_manager_;
    std::unique_ptr<ManagerInterface>      battery_manager_;
    std::unique_ptr<TempManagerInterface>  temp_manager_;

    array<data::StripeCounter, data::Sensors::kNumKeyence> keyence_stripe_counter_arr_;
    array<data::StripeCounter, data::Sensors::kNumKeyence> prev_keyence_stripe_count_arr_;
};

}}

#endif  // SENSORS_MAIN_HPP_
