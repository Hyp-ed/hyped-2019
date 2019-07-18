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
  Stepper* brake_1 = new Stepper(26, 27, log_, 1);
  Stepper* brake_2 = new Stepper(28, 29, log_, 2);
  Stepper* brake_3 = new Stepper(30, 31, log_, 3);
  Stepper* brake_4 = new Stepper(32, 33, log_, 4);
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
          
          brake_1->sendRetract();
          brake_2->sendRetract();
          brake_3->sendRetract();
          brake_4->sendRetract();

          brake_1->checkHome();
          brake_2->checkHome();
          brake_3->checkHome();
          brake_4->checkHome();

        } else if(!tlm_data_.nominal_braking_command && em_brakes_.brakes_retracted[0] &&
          em_brakes_.brakes_retracted[1] && em_brakes_.brakes_retracted[2] &&
          em_brakes_.brakes_retracted[3]) {

          brake_1->sendClamp();
          brake_2->sendClamp();
          brake_3->sendClamp();
          brake_4->sendClamp();

          brake_1->checkHome();
          brake_2->checkHome();
          brake_3->checkHome();
          brake_4->checkHome();

        }
        break;
      case data::State::kCalibrating:
        if(!em_brakes_.brakes_retracted[0] && !em_brakes_.brakes_retracted[1] &&
        !em_brakes_.brakes_retracted[2] && !em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendRetract();
          brake_2->sendRetract();
          brake_3->sendRetract();
          brake_4->sendRetract();

          em_brakes_.module_status = ModuleStatus::kReady;
          data_.setEmergencyBrakesData(em_brakes_);
        }

          brake_1->checkHome();
          brake_2->checkHome();
          brake_3->checkHome();
          brake_4->checkHome();

        break;
      case data::State::kAccelerating:
        brake_1->checkAccFailure();
        brake_2->checkAccFailure();
        brake_3->checkAccFailure();
        brake_4->checkAccFailure();

        break;
      case data::State::kNominalBraking:
        brake_1->sendClamp();
        brake_2->sendClamp();
        brake_3->sendClamp();
        brake_4->sendClamp();

        break;
      case data::State::kFinished:
        if(tlm_data_.nominal_braking_command && !em_brakes_.brakes_retracted[0] &&
          !em_brakes_.brakes_retracted[1] && !em_brakes_.brakes_retracted[2] &&
          !em_brakes_.brakes_retracted[3]) {
          
          brake_1->sendRetract();
          brake_2->sendRetract();
          brake_3->sendRetract();
          brake_4->sendRetract();

          brake_1->checkHome();
          brake_2->checkHome();
          brake_3->checkHome();
          brake_4->checkHome();

        } else if(!tlm_data_.nominal_braking_command && em_brakes_.brakes_retracted[0] &&
          em_brakes_.brakes_retracted[1] && em_brakes_.brakes_retracted[2] &&
          em_brakes_.brakes_retracted[3]) {

          brake_1->sendClamp();
          brake_2->sendClamp();
          brake_3->sendClamp();
          brake_4->sendClamp();

          brake_1->checkHome();
          brake_2->checkHome();
          brake_3->checkHome();
          brake_4->checkHome();

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