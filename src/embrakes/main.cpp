/*
* Author: Kornelija Sukyte
* Organisation: HYPED
* Date:
* Description: Entrypoint class to the embrake module, started in it's own thread.
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
{
  brake_1 = new Stepper(log_, 20);
  brake_2 = new Stepper(log_, 21);
  brake_3 = new Stepper(log_, 22);
  brake_4 = new Stepper(log_, 23);
}

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
        if(tlm_data_.nominal_braking_command && !em_brakes_.brakes_retracted[0] &&
          !em_brakes_.brakes_retracted[1] && !em_brakes_.brakes_retracted[2] &&
          !em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendRetract(0x0, 0x7d, 0x28);
          brake_2->sendRetract(0x0, 0x7d, 0x28);
          brake_3->sendRetract(0x0, 0x7d, 0x28);
          brake_4->sendRetract(0x0, 0x7d, 0x28);

        } else if(!tlm_data_.nominal_braking_command && em_brakes_.brakes_retracted[0] &&
          em_brakes_.brakes_retracted[1] && em_brakes_.brakes_retracted[2] &&
          em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendClamp();
          brake_2->sendClamp();
          brake_3->sendClamp();
          brake_4->sendClamp();

        }
        break;
      case data::State::kCalibrating:
        if(!em_brakes_.brakes_retracted[0] && !em_brakes_.brakes_retracted[1] &&
        !em_brakes_.brakes_retracted[2] && !em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendRetract(0x0, 0x7d, 0x28);
          brake_2->sendRetract(0x0, 0x7d, 0x28);
          brake_3->sendRetract(0x0, 0x7d, 0x28);
          brake_4->sendRetract(0x0, 0x7d, 0x28);
        }
        em_brakes_.module_status = ModuleStatus::kReady;
        data_.setEmergencyBrakesData(em_brakes_);
        break;
      case data::State::kNominalBraking:
        if(em_brakes_.brakes_retracted[0] && em_brakes_.brakes_retracted[1] &&
        em_brakes_.brakes_retracted[2] && em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendClamp();
          brake_2->sendClamp();
          brake_3->sendClamp();
          brake_4->sendClamp();
        }
        log_.INFO("Brakes", "Starting Nominal Braking");
        break;
      case data::State::kEmergencyBraking:

        // ???brakes engaged by cutting high power???

        log_.INFO("Brakes", "Starting Emergency Braking");

        // TODO(Kornelija): checkHome
        break;
      case data::State::kExiting:

        // ???clamp brakes when service propulsion is stopped???

        break;
      case data::State::kFinished:
        if(tlm_data_.nominal_braking_command && !em_brakes_.brakes_retracted[0] &&
          !em_brakes_.brakes_retracted[1] && !em_brakes_.brakes_retracted[2] &&
          !em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendRetract(0x0, 0x7d, 0x28);
          brake_2->sendRetract(0x0, 0x7d, 0x28);
          brake_3->sendRetract(0x0, 0x7d, 0x28);
          brake_4->sendRetract(0x0, 0x7d, 0x28);

        } else if(!tlm_data_.nominal_braking_command && em_brakes_.brakes_retracted[0] &&
          em_brakes_.brakes_retracted[1] && em_brakes_.brakes_retracted[2] &&
          em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendClamp();
          brake_2->sendClamp();
          brake_3->sendClamp();
          brake_4->sendClamp();

        }
        break;
      default:
        break;
    }
  }
  log_.INFO("Brakes", "Thread shutting down");
}
}  // namespace embrakes
}  // namespace hyped