/*
* Author: Gregor Konzett
* Organisation: HYPED
* Date: 31.3.2019
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
    finishedRetracting_(false),
    data_(data::Data::getInstance())
{
}

void Main::run()
{
  log_.INFO("Embrakes", "Thread started");

  System &sys = System::getSystem();

  // activate, step, push
  Pins pins[BREAKAMOUNT]={
    {8,9,11}
  };

  retractorManager = new RetractorManager(BREAKAMOUNT, pins,log_);

  while (sys.running_)
  {
    // Get the current state of embrakes and state machine modules from data
    em_brakes_ = data_.getEmergencyBrakesData();
    sm_data_ = data_.getStateMachineData();
    
    currentState = sm_data_.current_state;
    
  if (currentState == State::kCalibrating) { // Retract screw
    if (retractorManager->getStatus() == StatusCodes::IDLE) {
          log_.INFO("Embrakes", "Start Retracting");
          retractorManager->retract();
      } else if (retractorManager->getStatus() == StatusCodes::ERROR) {
        log_.ERR("Embrakes", "An error occured");
        em_brakes_.module_status = ModuleStatus::kCriticalFailure;
        data_.setEmergencyBrakesData(em_brakes_);

        finishedRetracting_ = true;
      } else if (retractorManager->getStatus() == StatusCodes::FINISHED && !finishedRetracting_) {
        log_.INFO("Embrakes", "Brakes are retracted");
        em_brakes_.module_status = ModuleStatus::kReady;
        data_.setEmergencyBrakesData(em_brakes_);
        
        finishedRetracting_ = true;
      }
    } else if (currentState == State::kNominalBraking) {
	    log_.INFO("Embrakes", "Starting Nominal Braking");
      em_brakes_.front_brakes = true;
	    em_brakes_.rear_brakes = true;
	    data_.setEmergencyBrakesData(em_brakes_);

    } else if (currentState == State::kEmergencyBraking) {
	    log_.INFO("Embrakes", "Starting Emergency Braking");
	    em_brakes_.front_brakes = true;
	    em_brakes_.rear_brakes = true;
	    data_.setEmergencyBrakesData(em_brakes_);
	  
	}
  }

  log_.INFO("Embrakes", "Thread shutting down");
}

}} // hyped::motor_control
