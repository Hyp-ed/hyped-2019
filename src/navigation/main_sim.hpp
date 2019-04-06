/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Main file for navigation simulation
 *
 *  Copyright 2019 HYPED
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *  except in compliance with the License. You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under
 *  the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef NAVIGATION_MAIN_SIM_HPP_
#define NAVIGATION_MAIN_SIM_HPP_

#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "navigation/single_imu_simulation.hpp"

namespace hyped {

using data::ImuData;
using utils::concurrent::Thread;
using utils::System;
using utils::Logger;
using utils::Timer;
using sensors::Imu;

namespace navigation {
class MainSim: public Thread {
  public:
    explicit MainSim(uint8_t id, Logger& log);
    void run() override;
  private:
    Logger& log;
};
}}  // namespace hyped navigation

#endif  // NAVIGATION_MAIN_SIM_HPP_
