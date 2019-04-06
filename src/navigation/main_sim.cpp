/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Main file for navigation simulation.
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

#include "main_sim.hpp"

namespace hyped {

namespace navigation {

MainSim::MainSim(uint8_t id, Logger& log_)
  : Thread(id, log_),
    log(log_)
{
  log.INFO("NAVIGATION", "Navigation simulation initialising");
}

void MainSim::run()
{
  // Start single IMU simulation
  SingleImuSimulation simulation = SingleImuSimulation();
  simulation.simulate(log);
}
}}  // namespace hyped navigation
