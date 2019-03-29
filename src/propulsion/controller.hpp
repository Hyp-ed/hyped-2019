/*
 * Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for fake IMUs
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef PROPULSION_CONTROLLER_HPP_
#define PROPULSION_CONTROLLER_HPP_

#include "propulsion/controller_interface.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "propulsion/can/can_sender.hpp"
#include "utils/io/can.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::io::can::Frame;
using utils::Logger;

namespace motor_control {

class Controller : public ControllerInterface {
 public:
  /**
   * @brief Construct a new Controller object
   * @param log
   * @param id
   */
  Controller(Logger& log, uint8_t id);
  /**
   * @brief Registers controller to recieve and transmit CAN messages.
   */
  void registerController() override;
  /**
   * @brief Apply configuration settings.
   */
  void configure() override;
  /**
   * @brief Check for errors or warnings, then enter operational state.
   */
  void enterOperational() override;
  /**
   * @brief Enter preoperational state.
   *
   */
  void enterPreOperational() override;
  /**
   * @brief Check controller state.
   */
  void checkState() override;
  /**
   * @brief Send the target velocity to the motor controller.
   * @param target_velocity - in rpm (calculated in speed calculator)
   */
  void sendTargetVelocity(int32_t target_velocity) override;
  /**
   * @brief Send a request to the motor controller to get the actual velocity.
   *
   */
  void updateActualVelocity() override;
  /**
   * @return int32_t - actual velocity of the motor
   */
  int32_t getVelocity() override;
  /**
   * @brief Set the controller to quickstop mode.
   */
  void quickStop() override;
  /**
   * @brief Check the error/warning regesters of the controller.
   */
  void healthCheck() override;
  /**
   * @return critical_failure_
   */
  bool getFailure() override;
  /**
   * @return state_
   */
  ControllerState getControllerState() override;
  /**
   * @return node_id_
   */
  uint8_t getNode_id();
  /**
   * @brief set critical_failure_
   */
  void setFailure(bool failure);

 private:
  Logger&           log_;
  data::Data&       data_;
  data::Motors      motor_data_;
  ControllerState   state_;
  uint8_t           node_id_;
  bool              critical_failure_;
  int32_t           actual_velocity_;
  CanSender         sender;
  Frame             sdo_message_;
  Frame             nmt_message_;

  // TODO(Iain): add predefined configuration messages and function codes
  const uint8_t           sample_message_data_[8] =
                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
};


}}  // namespace hyped::motor_control

#endif  // PROPULSION_CONTROLLER_HPP_
