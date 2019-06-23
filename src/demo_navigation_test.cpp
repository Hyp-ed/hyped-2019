/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 01/04/19
 * Description: Demo for IMU performance testing
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
#include "data/data.hpp"
#include "navigation/main_new.hpp"
#include "sensors/imu_manager.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

using hyped::data::Data;
using hyped::data::ModuleStatus;
using hyped::data::State;
using hyped::data::StateMachine;
using hyped::sensors::ImuManager;
using hyped::utils::concurrent::Thread;
using hyped::utils::System;
using hyped::utils::Logger;

int main(int argc, char* argv[])
{
  // system preparation
  System::parseArgs(argc, argv);
  System &sys = System::getSystem();
  Logger *log = new Logger(sys.verbose_nav, sys.debug_nav);
  static Data& data = Data::getInstance();

  // access to simulate state machine changes
  StateMachine state_machine = data.getStateMachineData();

  // set up sensors
  ImuManager imu_manager(*log);
  imu_manager.start();

  // get navigation up'n' runnin'
  Thread* nav = new hyped::navigation::Main(1, *log);
  nav->start();

  Thread::sleep(45000);


  nav->join();

  return 0;
}