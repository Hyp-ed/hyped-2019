/*
* Author:
* Organisation: HYPED
* Date:
* Description: Entrypoint class to the embrake module, started in it's own thread. Handles the logic to retract the brakes
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

namespace embrakes
{
Main::Main(uint8_t id, Logger &log)
  : Thread(id, log),
    log_(log),
    data_(data::Data::getInstance())
{ /* EMPTY */ }

void Main::run() {
  log_.INFO("Brakes", "Thread started");

  System &sys = System::getSystem();

  while (sys.running_) {
    // Get the current state of embrakes and state machine modules from data
    em_brakes_ = data_.getEmergencyBrakesData();
    sm_data_ = data_.getStateMachineData();
    tlm_data_ = data_.getTelemetryData();
    
    switch (sm_data_.current_state) {
      case data::State::kIdle:

        // TODO(Kornelija): retract/disengage brakes after command from GUI

        break;
      case data::State::kCalibrating:

        // TODO(Kornelija): calibrate brakes by retracting them

        em_brakes_.module_status = ModuleStatus::kReady;
        data_.setEmergencyBrakesData(em_brakes_);
        break;
      case data::State::kNominalBraking:

        // TODO(Kornelija): engage brakes

        log_.INFO("Brakes", "Starting Nominal Braking");

        // TODO(Kornelija): check whether button pressed before changing booleans in data structure

        em_brakes_.front_brakes = true;
        em_brakes_.rear_brakes = true;
        data_.setEmergencyBrakesData(em_brakes_);
        break;
      case data::State::kEmergencyBraking:

        // ???brakes engaged by cutting high power???

        log_.INFO("Brakes", "Starting Emergency Braking");

        // TODO(Kornelija): check whether button pressed before changing booleans in data structure

        em_brakes_.front_brakes = true;
        em_brakes_.rear_brakes = true;
        data_.setEmergencyBrakesData(em_brakes_);
        break;
      case data::State::kExiting:

        // ???clamp brakes when service propulsion is stopped???

        break;
      case data::State::kFinished:

        // ??? make sure brakes are retracted so the pod can slide off the rail???

        break;
      default:
        break;
    }
  }

  log_.INFO("Brakes", "Thread shutting down");
}

}} // hyped::motor_control
