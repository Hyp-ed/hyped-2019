/*
 * Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for the Motor Controller
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

#ifndef BRAKES_CONTROLLER_HPP_
#define BRAKES_CONTROLLER_HPP_

#include <atomic>

#include "embrakes/brakes_controller_interface.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "utils/io/can.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

#define TIMEOUT 70000

namespace hyped {

using std::atomic;
using utils::io::Can;
using utils::io::CanProccesor;
using utils::concurrent::Thread;
using utils::io::can::Frame;
using utils::Logger;
using utils::Timer;

namespace embrakes {

// Types of CANopen messagesused, these are used for CAN ID's
constexpr uint32_t kSdoReceive = 0x600;

constexpr uint32_t canIds[13] {0x80, 0x600, 0x580, 0x000, 0x700,
                              0x180, 0x200, 0x280, 0x300, 0x380,
                              0x400, 0x480, 0x500};

class Controller : public CanProccesor, public ControllerInterface {
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
   * @return state_
   */
  ControllerState getControllerState() override;
  /**
   * @brief { If this function returns true, the CAN message is ment for this CAN node }
   */
  bool hasId(uint32_t id, bool extended) override;
  /**
   * @brief set critical_failure_
   */
  void processNewData(utils::io::can::Frame& message) override;

  void sendData(uint8_t* message_data);

 private:
  /**
   * @brief compact function to send can message,
   *        while checking for critical failure.
   * @param message_template
   * @param len
   */
  bool sendControllerMessage(ControllerMessage message_template);
  /*
   * @brief Sends a CAN frame but waits for a reply
   */
  void sendSdoMessage(utils::io::can::Frame& message);
  /**
   * @brief set critical failure flag to true and write failure to data structure.
   */
  void throwCriticalFailure();

  Logger&                   log_;
  data::Data&               data_;
  data::EmergencyBrakes     brakes_data_;
  atomic<ControllerState>   state_;
  uint8_t                   node_id_;
  atomic<bool>              critical_failure_;
  Can                       &can_;
  Frame                     sdo_message_;
  uint64_t                  messageTimestamp;
  Timer                     timer;
  std::atomic<bool>         isSending;

};
}}  // namespace hyped::embrakes

#endif  // BRAKES_CONTROLLER_HPP_