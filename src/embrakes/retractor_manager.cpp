/*
* Author: Gregor Konzett
* Organisation: HYPED
* Date: 31.3.2019
* Description: Manages the Retractor Threads that retract the brakes 
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

#include "retractor_manager.hpp"

namespace hyped {
namespace embrakes {
RetractorManager::RetractorManager(uint breakAmount, Pins *pins,Logger& log) :  log_(log),
                                                                                sys_(System::getSystem())
{
  breakAmount_ = breakAmount;

  status = new std::atomic<StatusCodes>[breakAmount_];  
  retractors_ = new RetractorInterface*[breakAmount_]; 

  bool useFakeController = sys_.fake_embrakes;

  for (uint i = 0;i < breakAmount_;i++) {
    status[i] = StatusCodes::IDLE;                
                
    if (useFakeController) {
      retractors_[i] = new FakeRetractor(&status[i]);
    } else {
      retractors_[i] = new Retractor(pins[i].activate,pins[i].step,pins[i].push, &status[i]);
    }
  }
}

void RetractorManager::retract()
{
  for (uint i = 0; i < breakAmount_;i++) {
    status[i] = StatusCodes::STARTED;
    retractors_[i]->start();
    log_.INFO("Embrakes","Retracting brake");
  }
}

int RetractorManager::getStatus()
{
  StatusCodes statusCode = StatusCodes::FINISHED;

  for (uint i = 0; i < breakAmount_;i++) {
    if(status[i] < statusCode) {
    statusCode = status[i];
    }
  }

  return statusCode;
}

}}  // hyped::embrakes
