/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date:
 * Description:
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

    States state = Idle;

    while (isRunning && sys.running_) {
        log_.INFO("Motor", "Thread running");
        if (state == States::Idle) {  // Initialize motors
            log_.INFO("Motor", "State idle");

            if (!stateProcessor->isInitialized()) {
                stateProcessor->initMotors();
            }

            yield();
        } else if (state == States::Calibrating) {
            // Calculate slip values
            log_.INFO("Motor", "State Calibrating");
        } else if (state == States::Ready) {
            // Standby and wait
            log_.INFO("Motor", "State Ready");
        } else if (state == States::Accelerating) {
            // Accelerate the motors
            // TODO(gregor): Controller should handle the communication with the SpeedCalculator
            log_.INFO("Motor", "State Accelerating");
            stateProcessor->accelerate();
        } else if (state == States::Decelerating) {
            // Decelerate the motors (probably just shutting down the motors)
            log_.INFO("Motor", "State Decelerating");
            stateProcessor->decelerate();
        } else if (state == States::EmergencyBraking) {
            // Stop all motors
            log_.INFO("Motor", "State EmergencyBraking");
            stateProcessor->quickStopAll();
        } else if (state == States::Exiting) {
            // Move very slowly out of tube
            log_.INFO("Motor", "State Exiting");
        } else if (state == States::FailureStopped) {
            // Enter preoperational
            log_.INFO("Motor", "State FailureStopped");
            stateProcessor->enterPreOperational();
        } else if (state == States::Finished) {
            log_.INFO("Motor", "State Finished");
        } else if (state == States::RunComplete) {
            // Run complete
            log_.INFO("Motor", "State RunComplete");
        } else {
            // Unknown State
            log_.INFO("Motor", "State Unknown");
            isRunning = false;
        }
    }

    log_.INFO("Motor", "Thread shutting down");
}
}  // namespace motor_control
}  // namespace hyped
