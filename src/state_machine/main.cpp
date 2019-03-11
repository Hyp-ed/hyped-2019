/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 *
 *    Copyright 2019 HYPED
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

#include <cstdint>

#include "state_machine/hyped-machine.hpp"
#include "state_machine/main.hpp"

#include <cstdint>

#include "hyped-machine.hpp"
#include "main.hpp"

#include "../data/data.hpp"
#include "../utils/timer.hpp"
#include "../utils/system.hpp"

namespace hyped {
namespace state_machine {

Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      hypedMachine(log),
      timeout_(60000000),     // 60 seconds
      data_(data::Data::getInstance())
{ /* EMPTY */ }

/**
  *  @brief  Runs state machine thread.
  */

void Main::run()
{
  utils::System& sys = utils::System::getSystem();
  
  while (sys.running_) {
    comms_data_     = data_.getCommunicationsData();
    nav_data_       = data_.getNavigationData();
    sm_data_        = data_.getStateMachineData();
    motor_data_     = data_.getMotorData();
    batteries_data_ = data_.getBatteriesData();
    // sensors_data_   = data_.getSensorsData();

    switch (sm_data_.current_state) {
      case data::State::kIdle:
        if (checkCommsCriticalFailure()) break;   // TODO(anyone): discuss this transition again 
        if (checkInitialised())          break;
        break;
      case data::State::kCalibrating:
        if (checkCriticalFailure())      break;
        if (checkSystemsChecked())       break;
        break;
      case data::State::kReady:
        if (checkCriticalFailure())      break;
        if (checkOnStart())              break;
        break;
      case data::State::kAccelerating:
        if (checkCriticalFailure())      break;
        if (checkTimer())                break;
        if (checkMaxDistanceReached())   break;
        break;
      case data::State::kNominalBraking:
        if (checkCriticalFailure())      break;
        if (checkTimer())                break;
        if (checkVelocityZeroReached())  break;
        break;
      case data::State::kRunComplete:
        if (checkCriticalFailure())      break;
        if (checkOnExit())               break;
        break;
      case data::State::kExiting:
        if (checkCriticalFailure())      break;
        if (checkFinish())               break;
        break;
      case data::State::kEmergencyBraking:
        if (checkVelocityZeroReached())  break;
        break;
      // we cannot recover from these states
      case data::State::kInvalid:
        log_.ERR("STATE", "we are in Invalid state");
      case data::State::kFinished:
      case data::State::kFailureStopped:
      default:
        break;
    }

}} // namespace hyped::state_machine
