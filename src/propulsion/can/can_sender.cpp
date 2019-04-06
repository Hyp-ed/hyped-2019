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

#include "can_sender.hpp"

namespace hyped
{
namespace motor_control
{
CanSender::CanSender(Logger &log_, uint8_t node_id) : log_(log_),
                                                      node_id_(node_id),
                                                      can_(Can::getInstance()),
                                                      messageTimestamp(0)
{
    isSending = false;
    can_.start();
}

CanSender::CanSender(ControllerInterface* controller, uint8_t node_id, Logger& log_)
    :   node_id_(node_id),
        can_(Can::getInstance()),
        controller_(controller),
        messageTimestamp(0)
{
    isSending = false;
    can_.start();
}

bool CanSender::sendMessage(utils::io::can::Frame &message)
{
    log_.INFO("Motor", "Sending Message");
    can_.send(message);
    isSending = true;

    timer.start();
    messageTimestamp = timer.getTimeMicros();

    while (isSending) {
        if (timer.getTimeMicros() - messageTimestamp > TIMEOUT) {
            return false;
        }
    }

    return true;
}

void CanSender::registerController()
{
    can_.registerProcessor(this);
}

void CanSender::processNewData(utils::io::can::Frame &message)
{
    isSending = false;

    uint32_t id = message.id;
    if (id == kEmgyTransmit + node_id_) {
        controller_->processEmergencyMessage(message);
    } else if (id == kSdoTransmit + node_id_) {
        controller_->processSdoMessage(message);
    } else if (id == kNmtTransmit + node_id_) {
        controller_->processNmtMessage(message);
    } else {
        log_.ERR("Motor", "Controller %d: CAN message not recognised", node_id_);
    }
}

bool CanSender::hasId(uint32_t id, bool extended)
{
    for (uint32_t cobId : canIds) {
        if (cobId + node_id_ == id) {
            return true;
        }
    }

    return false;
}

bool CanSender::getIsSending()
{
    return isSending;
}
}  // namespace motor_control
}  // namespace hyped
