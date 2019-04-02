/*
 * Authors : HYPED
 * Organisation: HYPED
 * Date: 3. February 2018
 * Description:
 * This is the main executable for BeagleBone pod node
 *
 *    Copyright 2018 HYPED
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

#include "utils/logger.hpp"
#include "utils/system.hpp"

using hyped::utils::Logger;
using hyped::utils::System;

int main(int argc, char* argv[])
{
  System::parseArgs(argc, argv);
  System& sys = System::getSystem();
  Logger log_system(sys.verbose, sys.debug);
  Logger log_motor(sys.verbose_motor, sys.debug_motor);
  Logger log_nav(sys.verbose_nav, sys.debug_nav);
  Logger log_sensor(sys.verbose_sensor, sys.debug_sensor);
  Logger log_state(sys.verbose_state, sys.debug_state);
  Logger log_tlm(sys.verbose_tlm, sys.debug_tlm);

  log_system.INFO("MAIN", "Starting BBB with %d modules", 5);
  log_system.DBG("MAIN", "DBG0");
  log_system.DBG1("MAIN", "DBG1");
  log_system.DBG2("MAIN", "DBG2");
  log_system.DBG3("MAIN", "DBG3");

  return 0;
}
