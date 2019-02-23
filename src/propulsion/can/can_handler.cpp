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

#include "can_handler.hpp"

namespace hyped
{

namespace motor_control
{

CanHandler::CanHandler(Logger &log_) : log_(log_)
{
    log_.INFO("Motor", "CanSender initialized");

    //Start Sending Thread
    sender = new CanSender(log_);
    sender->start();
}

void CanHandler::pushSdoMessageToQueue(utils::io::can::Frame &message)
{
    //log_.INFO("Motors",message.id+" before queue");

    queue.push(message);

    log_.INFO("Motor", "send message");

    utils::io::can::Frame front = queue.front();

    log_.INFO("Motor", front.id + " from queue");
}

void CanHandler::processNewData(utils::io::can::Frame &message)
{
}

bool CanHandler::hasId(uint32_t id, bool extended)
{
    return true;
}

void CanHandler::registerController()
{
}
} // namespace motor_control
} // namespace hyped
