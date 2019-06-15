/*
 * Author: Lukas Schaefer, Neil McBlane
 * Organisation: HYPED
 * Date: 05/04/2019
 * Description: Main file for navigation class.
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

#include <iostream>

#include "navigation/main.hpp"

namespace hyped {
namespace navigation {

  Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      log_(log),
      sys_(System::getSystem()),
      nav_(log, sys_.axis) {}

  void Main::run()
  {
    log_.INFO("NAV", "Axis: %d", sys_.axis);
    log_.INFO("NAV", "Navigation waiting for calibration");

    Data& data = Data::getInstance();
    // wait for calibration state for calibration
    StateMachine state_machine = data.getStateMachineData();
    while (state_machine.current_state != State::kCalibrating) {
      // wait 100ms
      Thread::sleep(100);
      state_machine = data.getStateMachineData();
    }

    nav_.calibrateGravity();

    // wait for accelerating state
    log_.INFO("NAV", "Navigation waiting for acceleration");
    state_machine = data.getStateMachineData();
    while (state_machine.current_state != State::kAccelerating) {
      // wait 1ms
      Thread::sleep(1);
      state_machine = data.getStateMachineData();
    }

    log_.INFO("NAV", "Navigation starting");
    nav_.initTimestamps();
    while (sys_.running_) {
      nav_.navigate();
    }
  }



}}  // namespace hyped::navigation
