/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: March 2019
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

#include "utils.hpp"

namespace hyped {

namespace telemetry {

ProtoModuleStatus Utils::moduleStatusEnumConversion(DataModuleStatus status)
{
    switch (status) {
        case data::ModuleStatus::kStart:
            return telemetry_data::ClientToServer::START;
        case data::ModuleStatus::kInit:
            return telemetry_data::ClientToServer::INIT;
        case data::ModuleStatus::kReady:
            return telemetry_data::ClientToServer::READY;
        case data::ModuleStatus::kCriticalFailure:
            return telemetry_data::ClientToServer::CRITICAL_FAILURE;
        default:
            // TODO(neil): throw error or something
            return telemetry_data::ClientToServer::CRITICAL_FAILURE;
    }
}

ProtoState Utils::stateEnumConversion(DataState state)
{
    switch (state) {
        case data::State::kIdle:
            return telemetry_data::ClientToServer::StateMachine::IDLE;
        case data::State::kCalibrating:
            return telemetry_data::ClientToServer::StateMachine::CALIBRATING;
        case data::State::kReady:
            return telemetry_data::ClientToServer::StateMachine::READY;
        case data::State::kAccelerating:
            return telemetry_data::ClientToServer::StateMachine::ACCELERATING;
        case data::State::kNominalBraking:
            return telemetry_data::ClientToServer::StateMachine::NOMINAL_BRAKING;
        case data::State::kEmergencyBraking:
            return telemetry_data::ClientToServer::StateMachine::EMERGENCY_BRAKING;
        case data::State::kRunComplete:
            return telemetry_data::ClientToServer::StateMachine::RUN_COMPLETE;
        case data::State::kFailureStopped:
            return telemetry_data::ClientToServer::StateMachine::FAILURE_STOPPED;
        case data::State::kExiting:
            return telemetry_data::ClientToServer::StateMachine::EXITING;
        case data::State::kFinished:
            return telemetry_data::ClientToServer::StateMachine::FINISHED;
        default:
            // TODO(neil): throw error or something
            return telemetry_data::ClientToServer::StateMachine::INVALID;
    }
}

}  // namespace telemetry
}  // namespace hyped
