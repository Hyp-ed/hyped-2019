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
      initialized(false),
      sys_(System::getSystem()),
      motorAmount(motorAmount)
{

    log_.DBG1("Motor", "StateProcessor constructor was called");
    useTestControllers = false;//sys_.fake_motors;

    //controllers = new ControllerInterface[motorAmount];

    if (useTestControllers) //Use the test controller implementation
    {
    }
    else //Use real controllers
    {
        for (int i = 0; i < motorAmount; i++)
        {
            //controllers[i] = new Controller();
        }
    }
}

void StateProcessor::initMotors()
{
    //Register controllers on CAN bus
    registerControllers();

    //Configure controller parameters
    configureControllers();

    //TODO: Handle errors

    //If no error
    prepareMotors();

    initialized = true;
}

void StateProcessor::registerControllers()
{
}

void StateProcessor::configureControllers()
{
}

void StateProcessor::prepareMotors()
{
}

void StateProcessor::enterPreOperational()
{
}

void StateProcessor::accelerate()
{
}

void StateProcessor::decelerate()
{
}

void StateProcessor::sendTargetVelocity(int32_t target_velocity)
{
}

MotorVelocity StateProcessor::requestActualVelocity()
{
    return MotorVelocity();
}

void StateProcessor::quickStopAll()
{
}

void StateProcessor::healthCheck()
{
}

bool StateProcessor::getFailure()
{
    return false;
}

bool StateProcessor::isInitialized()
{
    return this->initialized;
}

void StateProcessor::setInitialized(bool initialized)
{
    this->initialized = initialized;
}

} // namespace motor_control
} // namespace hyped