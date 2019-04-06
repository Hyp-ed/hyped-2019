
/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 01/04/19
 * Description: Demo for Single IMU Navigation
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

#include "navigation/main.hpp"
#include "sensors/imu_manager.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

using sensors::ImuManager;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::navigation::Main;

int main(int argc, char* argv[])
{
  System::parseArgs(argc, argv);
  System &sys = System::getSystem();
  Logger* log_nav = new Logger(sys.verbose_nav, sys.debug_nav);
  
  // Initialise sensors
  ImuManager imu_manager(log_nav);
  imu_manager.start();

  Main* main = new Main(1, *log_nav);
  main->run();

  while (sys.running_)
	;

  return 0;
}
