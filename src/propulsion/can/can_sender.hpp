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

#ifndef HYPED_2019_CANSENDER_HPP
#define HYPED_2019_CANSENDER_HPP

#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "sender_interface.hpp"
#include <atomic>
#include <iostream>

namespace hyped
{
namespace motor_control
{
using utils::Logger;
using utils::io::Can;
using utils::io::CanProccesor;

// Types of CANopen messages, these are used for CAN ID's
constexpr uint32_t kEmgyTransmit = 0x80;
constexpr uint32_t kSdoReceive = 0x600;
constexpr uint32_t kSdoTransmit = 0x580;
constexpr uint32_t kNmtReceive = 0x000;
constexpr uint32_t kNmtTransmit = 0x700;
constexpr uint32_t kPdo1Transmit = 0x180;
constexpr uint32_t kPdo1Receive = 0x200;
constexpr uint32_t kPdo2Transmit = 0x280;
constexpr uint32_t kPdo2Receive = 0x300;
constexpr uint32_t kPdo3Transmit = 0x380;
constexpr uint32_t kPdo3Receive = 0x400;
constexpr uint32_t kPdo4Transmit = 0x480;
constexpr uint32_t kPdo4Receive = 0x500;

constexpr uint32_t canIds[13]{0x80, 0x600, 0x580, 0x000, 0x700,
                              0x180, 0x200, 0x280, 0x300, 0x380,
                              0x400, 0x480, 0x500};

class CanSender : public CanProccesor, public SenderInterface
{

public:
  /**
		 * @brief { Initialise the CanSender with the logger and the id }
		 */
  CanSender(Logger &log_, uint8_t id);

  /**
		 * @brief { Initialise the CanSender with the logger, the id and the controller as an attribute,
     * to access it's attributes }
		 */
  //CanSender(ControllerInterface* controller,uint_8_t id,Logger& log_);

  /**
		 * @brief { Sends CAN messages }
		 */
  void sendMessage(utils::io::can::Frame &message) override;

  /**
		 * @brief { Registers the controller to process incoming CAN messages }
		 */
  void registerController() override;

  /**
		 * @brief { This function processes incoming CAN messages }
		 */
  void processNewData(utils::io::can::Frame &message) override;

  /**
		 * @brief { If this function returns true, the CAN message is ment for this CAN node }
		 */
  bool hasId(uint32_t id, bool extended) override;

  /**
		 * @brief { Return if the can_sender is sending a CAN message right now }
		 */
  bool getIsSending();

private:
  Logger log_;
  uint8_t node_id_;
  Can &can_;
  std::atomic<bool> isSending;
};
} // namespace motor_control
} // namespace hyped

#endif // HYPED_2019_CANSENDER_HPP