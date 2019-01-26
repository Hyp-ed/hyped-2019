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

#ifndef BEAGLEBONE_BLACK_MOTOR_CONTROL_STATEPROCESSOR_HPP_
#define BEAGLEBONE_BLACK_MOTOR_CONTROL_STATEPROCESSOR_HPP_


#include "utils/logger.hpp"
#include "utils/system.hpp"

#include "StateProcessorInterface.hpp"

namespace hyped {

namespace motor_control {

    using utils::System;
    using utils::Logger;

	struct MotorVelocity {
		int32_t velocity_1;
		int32_t velocity_2;
		int32_t velocity_3;
		int32_t velocity_4;
		int32_t velocity_5;
		int32_t velocity_6;
	};

	class StateProcessor : StateProcessorInterface {
	public:
		explicit StateProcessor(Logger& log);

		/**
		 * @brief { Initializes the motor controllers and prepare the motors }
		 */
		void initMotors() override;

		void enterPreOperational() override;
		/**
          *  @brief  { Set target velocity for each controller }
          *
          *  @param[in] { Target velocity calculated in Main }
          */
		void sendTargetVelocity(int32_t target_velocity) override;
		/**
          *  @brief  { Read actual velocity from each controller }
          *
          *  @return { Motor velocity struct }
          */
		MotorVelocity requestActualVelocity() override;
		/*
         *  @brief  { Sets all controllers into quickStop mode. Use in case of critical failure }
         */
		void quickStopAll() override;
		/*
         *  @brief { Checks the error status and warning status in each controller object
         *           Sets critical failure flag true if there is an error }
         */
		void healthCheck() override;
		/*
         *  @return { Critical failure flag }
         */
		bool getFailure() override;

	private:
		/**
          *   @brief  Registers the motor controllers to the can
          */
		void registerControllers() override;
		/**
          *   @brief  Applies configuration settings and sets controllers to Operational mode
          */
		void configureControllers() override;
		/**
          *   @brief  { Controllers are entered into Operational mode }
          */
		void prepareMotors();
		/**
          *   @brief  Sets controllers to pre operational mode
          */


		int motorAmount;
		bool initialized;
		bool useTestControllers;
	    Logger& log_;
	    System& sys_;
	    ControllerInterface* controllers[];
	};

}}  

#endif
