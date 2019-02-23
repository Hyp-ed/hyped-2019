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

#ifndef HYPED_2019_CANHANDLER_HPP
#define HYPED_2019_CANHANDLER_HPP

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <queue>
#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "can_sender.hpp"
#include "propulsion/can/sender_interface.hpp"

namespace hyped
{

namespace motor_control
{
using utils::Logger;
using utils::concurrent::Thread;
using utils::io::Can;
using utils::io::CanProccesor;

class CanHandler : public CanProccesor, public SenderInterface
{
    friend Can;

  public:
    CanHandler(Logger &log_);
    //CanSender(ControllerInterface* controller);

    void pushSdoMessageToQueue(utils::io::can::Frame &message) override;

    void registerController() override;

    void processNewData(utils::io::can::Frame &message) override;

    bool hasId(uint32_t id, bool extended) override;

  protected:
    std::mutex queueMutex;
    std::condition_variable queueConditionVar;
    bool processingMessage;
    Logger &log_;
    std::queue<utils::io::can::Frame> queue;
    Thread *sender;
};

} // namespace motor_control
} // namespace hyped

#endif //HYPED_2019_CANHANDLER_HPP
