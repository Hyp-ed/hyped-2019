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

#include "main.hpp"

namespace hyped
{

namespace embrakes
{
    Main::Main(uint8_t id, Logger &log)
	: Thread(id, log),
	  log_(log)
{
	
}

void Main::run()
{
	log_.INFO("Embrakes", "Thread started");

	System &sys = System::getSystem();

	Data stateMachineData = Data::getInstance();

	Pins pins[2]={
		{1,1},
		{2,2}
	};

	retractor = new RetractorManager(pins,log_);

	retractor->retract();

	/*while (sys.running_)
	{
		log_.INFO("Embrakes", "Thread running");

        //Get the current state of the system from the state machine's data
        currentState = stateMachineData.getStateMachineData().current_state;

        int x = 2;

		if (currentState == State::kCalibrating) //Retract screw
		{
            if(!retracted) {
                
            }
		}
	}*/

	log_.INFO("Embrakes", "Thread shutting down");
}

bool Main::isRetracted()
{
    return retracted;
}

} // namespace motor_control
} // namespace hyped
