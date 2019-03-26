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

#include "state_processor.hpp"

namespace hyped
{

namespace motor_control
{

StateProcessor::StateProcessor(int motorAmount, Logger &log)
    : log_(log),
      sys_(System::getSystem()),
      motorAmount(motorAmount),
      initialized(false),
      criticalError(false),
      servicePropulsionSpeed(10)
{
    log_.DBG1("Motor", "StateProcessor constructor was called");
    useTestControllers = true;  // sys_.fake_motors;

    // controllers = new ControllerInterface[motorAmount];

    if (useTestControllers) {  // Use the test controllers implementation
        for (int i = 0; i < motorAmount; i++) {
            controllers[i] = new FakeController(log_, i, false);
        }
    } else {  // Use real controllers
        for (int i = 0; i < motorAmount; i++) {
            // controllers[i] = new Controller();
        }
    }
}

void StateProcessor::initMotors()
{
    // Register controllers on CAN bus
    registerControllers();

    // Configure controllers parameters
    configureControllers();

    bool error = false;

    for (int i = 0;i < motorAmount;i++) {
        if (controllers[i]->getFailure()) {
            error = true;
            break;
        }
    }

    if (error) {
        prepareMotors();
        initialized = true;
    } else {
        criticalError = true;
    }
}

void StateProcessor::registerControllers()
{
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->registerController();
    }
}

void StateProcessor::configureControllers()
{
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->configure();
    }
}

void StateProcessor::prepareMotors()
{
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->enterOperational();
    }
}

void StateProcessor::enterPreOperational()
{
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->enterPreOperational();
    }
}

void StateProcessor::accelerate()
{
    int speed = 0;
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->sendTargetVelocity(speed);
    }
}

void StateProcessor::quickStopAll()
{
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->quickStop();
    }
}

void StateProcessor::healthCheck()
{
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->healthCheck();
    }
}

bool StateProcessor::getFailure()
{
    for (int i = 0;i < motorAmount; i++) {
        if (controllers[i]->getFailure()) {
            return true;
        }
    }

    return false;
}

void StateProcessor::servicePropulsion()
{
    for (int i = 0;i < motorAmount; i++) {
        controllers[i]->sendTargetVelocity(servicePropulsionSpeed);
    }
}

bool StateProcessor::isInitialized()
{
    return this->initialized;
}

bool StateProcessor::isCriticalFailure()
{
    return this->criticalError;
}

}  // namespace motor_control
}  // namespace hyped
