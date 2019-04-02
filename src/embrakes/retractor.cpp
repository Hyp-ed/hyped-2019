/*
* Author: Gregor Konzett
* Organisation: HYPED
* Date: 31.3.2019
* Description: Implements functions to retract the embrakes in it's own thread
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

#include "retractor.hpp"

namespace hyped
{
namespace embrakes
{   
Retractor::Retractor(uint32_t activate,uint32_t step, uint32_t pushButton, std::atomic<StatusCodes> *status)
{
  step_=step;
  status_=status;
  activate_=activate;
  pushButton_=pushButton;

  // Period calculation
  rpm = 10;
  float frequency = rpm * STEPS / 60;
  period = (1/frequency)*1000/2;

  startTime = time(NULL);
}

void Retractor::run()
{ 
  bool active=false;

  // Init GPIO Pins
  GPIO activatePin(activate_,Direction::kOut);
  GPIO stepPin(step_,Direction::kOut);
  GPIO pushButtonPin(pushButton_,Direction::kIn);

  activatePin.set();

  // Waits for 1 second to start the retracting process
  while(time(NULL)<startTime+1);

  while(*status_ == StatusCodes::STARTED) {
    if(active) {
      log_.INFO("Embrake", "Retract clear");
      stepPin.clear();
    } else {
      log_.INFO("Embrake", "Retract set");
      stepPin.set();
    }

    active = !active;

    if(pushButtonPin.read()==1) {
      *status_ = StatusCodes::FINISHED;
      break;
    }

    sleep(period);
  }

  log_.INFO("Embrake", "Retracted");

  stepPin.clear();
}
}}  // hyped::embrakes
