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

#ifndef EMBRAKES_MAIN_HPP_
#define EMBRAKES_MAIN_HPP_

#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "data/data.hpp"
#include "retractor_manager.hpp"

#define BREAKAMOUNT 1

namespace hyped {

using utils::concurrent::Thread;
using data::Data;
using data::State;
using utils::Logger;
using utils::System;
using data::ModuleStatus;
using data::EmergencyBrakes;

namespace embrakes {

/*
 * @description This module handles the interaction with the embrakes. 
 * If the current state of the state machine is kCalibrating, the Main class calls the retract function of the RetractorManager
 * class to start the retracting process. This includes the initialization of the GPIO pins and the actual retracting process,
 * which is done in it's own thread in the Retractor class for every embrake (There are 4)
 * After the brakes are fully retracted by the Retractor class the Main class is setting the module_status to kReady. If the brakes could not be fully retracted,
 * the module_status is set to kCriticalFailure.
*/
class Main : public Thread 
{
  public:
    /*
    * @brief Initialises essential variables
    */
    Main(uint8_t id, Logger &log);

    /*
    * @brief Checks for State kCalibrating to start retracting process
    */
    void run() override;

  private:
    State currentState;
    RetractorManager *retractorManager;
    Logger &log_;
    bool finishedRetracting_;
};

}}

#endif  // EMBRAKES_MAIN_HPP_
