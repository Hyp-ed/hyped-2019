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

#ifndef HYPED_STATEPROCESSOR_HPP_
#define HYPED_STATEPROCESSOR_HPP_

#include "utils/logger.hpp"
#include "utils/system.hpp"

#include "propulsion/state_processor_interface.hpp"

namespace hyped
{

namespace motor_control
{

using utils::Logger;
using utils::System;

class StateProcessor : public StateProcessorInterface
{
  public:
	StateProcessor(int motorAmount, Logger &log);

	/**
		 * @brief { Sends the desired settings to the motors }
		 */
	void initMotors() override;

	/**
		 * @brief { Changes the state of the motor controller to preOperational }
		 */
	void enterPreOperational() override;

	/**
		 * @brief { Stops all motors }
		 */
	void quickStopAll() override;

	/**
		 * @brief { Checks the motor controller's health }
		 */
	void healthCheck() override;

	/**
		 * @brief { Checks if the motor controller's error registers }
		 */
	bool getFailure() override;

	/**
		 * @brief { Tells the controllers to start accelerating the motors }
		 */
	void accelerate() override;

	/**
		 * @brief { Returns if the motors are initialised already }
		 */
	bool isInitialized() override;

  protected:
	/**
		 * @brief { Registers the controllers to handle CAN transmissions }
		 */
	void registerControllers() override;

	/**
		 * @brief { Configures the controllers }
		 */
	void configureControllers() override;

	/**
		 * @brief { Send settings data to the motors }
		 */
	void prepareMotors() override;

	bool useTestControllers;
	Logger &log_;
	System &sys_;
	int motorAmount;
	bool initialized;
	//ControllerInterface* controllers[];
};

} // namespace motor_control
} // namespace hyped

#endif // HYPED_STATEPROCESSOR_HPP_
