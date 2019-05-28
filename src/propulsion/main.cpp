/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date: 1.4.2019
 * Description: Main entrypoint to motor control module
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "main.hpp"

namespace hyped
{

namespace motor_control
{
Main::Main(uint8_t id, Logger &log)
  : Thread(id, log),
    isRunning(true),
    log_(log)
{
  log_.INFO("Motor", "Logger constructor was called");

  stateProcessor = new StateProcessor(6, log);
}

void Main::run()
{
  log_.INFO("Motor", "Thread started");

  System &sys = System::getSystem();

  Data stateMachineData = Data::getInstance();
  Motors motorData = stateMachineData.getMotorData();

  while (isRunning && sys.running_) {
    // Get the current state of the system from the state machine's data

    currentState = stateMachineData.getStateMachineData().current_state;

    if (currentState == State::kIdle) {  // Initialize motors
      log_.INFO("Motor", "State idle");

      if (motorData.module_status != ModuleStatus::kInit) {
        motorData.module_status = ModuleStatus::kInit;
      }

      yield();
      } else if (currentState == State::kCalibrating) {
        // Calculate slip values
        log_.INFO("Motor", "State Calibrating");

        if (!stateProcessor->isInitialized()) {
          stateProcessor->initMotors();
          if (stateProcessor->isCriticalFailure()) {
            motorData.module_status = ModuleStatus::kCriticalFailure;
            isRunning = false;
          }
        }
        if (stateProcessor->isInitialized() &&
              motorData.module_status != ModuleStatus::kReady) {
          motorData.module_status = ModuleStatus::kReady;
        }
        yield();
        } else if (currentState == State::kReady) {
          // Standby and wait
          log_.INFO("Motor", "State Ready");
          yield();
        } else if (currentState == State::kAccelerating) {
          // Accelerate the motors
          log_.INFO("Motor", "State Accelerating");
          stateProcessor->accelerate();
        } else if (currentState == State::kNominalBraking) {
          // Stop all motors
          log_.INFO("Motor", "State NominalBraking");
          stateProcessor->quickStopAll();
        } else if (currentState == State::kEmergencyBraking) {
          // Stop all motors
          log_.INFO("Motor", "State EmergencyBraking");
          stateProcessor->quickStopAll();
        } else if (currentState == State::kExiting) {
          // Move very slowly out of tube
          log_.INFO("Motor", "State Exiting");
          stateProcessor->servicePropulsion();
        } else if (currentState == State::kFailureStopped) {
          // Enter preoperational
          log_.INFO("Motor", "State FailureStopped");
          stateProcessor->enterPreOperational();
        } else if (currentState == State::kFinished) {
          log_.INFO("Motor", "State Finished");
          stateProcessor->enterPreOperational();
        } else if (currentState == State::kRunComplete) {
          // Run complete
          log_.INFO("Motor", "State RunComplete");
          stateProcessor->quickStopAll();
        } else {
          // Unknown State
          log_.INFO("Motor", "State Unknown");
          isRunning = false;
          stateProcessor->quickStopAll();
        }
    }

    log_.INFO("Motor", "Thread shutting down");
}
}  // namespace motor_control
}  // namespace hyped
