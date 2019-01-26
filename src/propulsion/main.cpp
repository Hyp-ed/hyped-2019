/*
 * Author: 
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

namespace hyped {

namespace motor_control {
	Main::Main()
    {
        std::cout << "Main default constructor was called" << std::endl;
	}

	Main::Main(uint8_t id, Logger& log)
		: Thread(id, log),
		isRunning(true)
	{
        std::cout << "Logger constructor was called" << std::endl;

        stateProcessor = new StateProcessor(log);
	}

	void Main::run()
	{
		std::cout << "Thread started" << std::endl;

		System& sys = System::getSystem();

		States state;// = Idle;

		while(isRunning && sys.running_) 
		{
			std::cout << "Thread running " << state << std::endl;
			if(state == States::Idle) 
			{
				std::cout << "State idle" << std::endl;

				if(!stateProcessor->isInitialized())
                {
                    stateProcessor->initMotors();
                }

				yield();
			} 
			else if(state == States::Accelerating)
			{
				//TODO: Controller should handle the communication with the SpeedCalculator
			    std::cout << "State Accelerating" << std::endl;
			}
			else if(state == States::Decelerating)
			{
				std::cout << "State Decelerating" << std::endl;
			}
			else if(state == States::EmergencyBraking)
			{
				std::cout << "State EmergencyBraking" << std::endl;
			}
			else if(state == States::Exiting)
			{
				std::cout << "State Exiting" << std::endl;
			}
			else if(state == States::FailureStopped)
			{
				std::cout << "State FailureStopped" << std::endl;
			}
			else if(state == States::Finished)
			{
				std::cout << "State Finished" << std::endl;
			}
			else if(state == States::RunComplete)
			{
				std::cout << "State RunComplete" << std::endl;
			}
			else 
			{
				std::cout << "Unknown state" << std::endl;
				isRunning = false;
			}

			Thread::sleep(500);
		}
		
		std::cout << "Thread shutting down" << std::endl;
	}
}} 
