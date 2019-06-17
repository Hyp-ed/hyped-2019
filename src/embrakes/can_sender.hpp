/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date: 1.4.2019
 * Description: Handles the communication with the CAN Bus
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

#ifndef BRAKES_CAN_SENDER_HPP_
#define BRAKES_CAN_SENDER_HPP_

#include <atomic>
#include <iostream>
#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"

#define TIMEOUT 5000

namespace hyped
{
namespace embrakes
{
using utils::Logger;
using utils::io::Can;
using utils::io::CanProccesor;
using utils::Timer;

class CanSender : public CanProccesor
{
  public:
    /**
       * @brief { Initialise the CanSender with the logger and the id }
       */
    CanSender(Logger &log_, uint8_t node_id);

    /**
       * @brief { Initialise the CanSender with the logger, the id and the controller as an attribute,
       * to access it's attributes }
       */
    CanSender(ControllerInterface* controller, uint8_t node_id, Logger& log_);

    /**
       * @brief { Sends CAN messages }
       */
    bool sendMessage(utils::io::can::Frame &message) override;

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
    bool getIsSending() override;

  private:
    Logger log_;
    uint8_t node_id_;
    Can &can_;
    std::atomic<bool> isSending;
    ControllerInterface *controller_;
    Timer timer;
    uint64_t messageTimestamp;

    const uint32_t kEmgyTransmit          = 0x80;
    const uint32_t kSdoTransmit           = 0x580;
    const uint32_t kNmtTransmit           = 0x700;
};
}  // namespace embrakes
}  // namespace hyped

#endif  // BRAKES_CAN_SENDER_HPP_