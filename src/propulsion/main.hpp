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

#ifndef HYPED_MOTORCONTROL_MAIN_HPP_
#define HYPED_MOTORCONTROL_MAIN_HPP_

#include "utils/concurrent/thread.hpp"
#include "utils/concurrent/barrier.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "./can/can_handler.hpp"

#include "state_processor.hpp"

namespace hyped {
	using utils::concurrent::Thread;
	using utils::Logger;
	using utils::System;
namespace motor_control {
	class Main: public Thread {
		public:
			Main(uint8_t id, Logger& log);
			
			/**
			 * Gets called when Thread is started. Entrypoint in motor control
			 * Includes the event loop for motor control
			 * Responds to the different states with appropriate actions
			 * */
			void run() override;

		private:
			bool isRunning;
			Logger& log_;
			StateProcessor* stateProcessor;
	};


	/**
	 * States (for now):
	 * 	Idle 		
	 * 	Calibrating
	 * 	Ready
	 * 	Accelerating  		
	 * 	Braking		
	 * 	EmergencyBraking	
	 * 	FailureStopped		
	 * 	RunComplete			
	 * 	Exiting				
	 * 	Finished			
	 * */
	enum States {Idle,Calibrating,Ready,Accelerating,Decelerating,EmergencyBraking,FailureStopped,RunComplete,Exiting,Finished};
}}  

#endif //HYPED_MOTORCONTROL_MAIN_HPP_
