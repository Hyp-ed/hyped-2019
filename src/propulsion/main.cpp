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

#include "propulsion/main.hpp"

namespace hyped
{

namespace motor_control
{
Main::Main(uint8_t id, Logger &log)
  : Thread(id, log),
    isRunning(true),
    log_(log),
    data_(Data::getInstance())
{
  stateProcessor = new StateProcessor(6, log);
}

void Main::run()
{
  log_.INFO("Motor", "Thread started");

  System &sys = System::getSystem();

  Motors motorData = data_.getMotorData();

  while (isRunning && sys.running_) {
    // Get the current state of the system from the state machine's data

    currentState = data_.getStateMachineData().current_state;

    if (currentState == State::kIdle) {  // Initialize motors
      if (previousState != currentState)
        log_.DBG1("Motor", "State idle");
      previousState = currentState;
      if (motorData.module_status != ModuleStatus::kInit) {
        motorData.module_status = ModuleStatus::kInit;
        data_.setMotorData(motorData);
      }

      yield();
    } else if (currentState == State::kCalibrating) {
        // Calculate slip values
        if (previousState != currentState)
          log_.DBG1("Motor", "State Calibrating");
        previousState = currentState;
        if (!stateProcessor->isInitialized()) {
          stateProcessor->initMotors();
          if (stateProcessor->isCriticalFailure()) {
            motorData.module_status = ModuleStatus::kCriticalFailure;
            data_.setMotorData(motorData);
            isRunning = false;
          }
        }
        if (stateProcessor->isInitialized() && motorData.module_status != ModuleStatus::kReady) {
          motorData.module_status = ModuleStatus::kReady;
          data_.setMotorData(motorData);
        }
        yield();
    } else if (currentState == State::kReady) {
          // Standby and wait
          if (previousState != currentState)
            log_.DBG1("Motor", "State Ready");
          previousState = currentState;
          yield();
    } else if (currentState == State::kAccelerating) {
          // Accelerate the motors
          if (previousState != currentState)
            log_.DBG1("Motor", "State Accelerating");
          previousState = currentState;
          stateProcessor->accelerate();
    } else if (currentState == State::kNominalBraking) {
          // Stop all motors
          if (previousState != currentState)
            log_.DBG1("Motor", "State NominalBraking");
          previousState = currentState;
          stateProcessor->quickStopAll();
    } else if (currentState == State::kEmergencyBraking) {
          // Stop all motors
          if (previousState != currentState)
            log_.DBG1("Motor", "State EmergencyBraking");
          previousState = currentState;
          stateProcessor->quickStopAll();
    } else if (currentState == State::kExiting) {
          // Move very slowly out of tube
          if (previousState != currentState)
            log_.DBG1("Motor", "State Exiting");
          previousState = currentState;
          Telemetry telem = data_.getTelemetryData();
          if (telem.service_propulsion_go) {
            stateProcessor->servicePropulsion();
            log_.DBG1("MOTOR", "Service propulsion active");
          }
    } else if (currentState == State::kFailureStopped) {
          // Enter preoperational
          if (previousState != currentState)
            log_.DBG1("Motor", "State FailureStopped");
          previousState = currentState;
          stateProcessor->enterPreOperational();
    } else if (currentState == State::kFinished) {
          if (previousState != currentState)
            log_.DBG1("Motor", "State Finished");
          previousState = currentState;
          stateProcessor->enterPreOperational();
    } else if (currentState == State::kRunComplete) {
          // Run complete
          if (previousState != currentState)
            log_.DBG1("Motor", "State RunComplete");
          previousState = currentState;
          stateProcessor->quickStopAll();
    } else {
          // Unknown State
          if (previousState != currentState)
            log_.DBG1("Motor", "State Unknown");
          previousState = currentState;
          isRunning = false;
          stateProcessor->quickStopAll();
    }
  }

  log_.INFO("Motor", "Thread shutting down");
}
}  // namespace motor_control
}  // namespace hyped
