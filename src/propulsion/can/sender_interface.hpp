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

#ifndef HYPED_2019_SENDERINTERFACE_HPP
#define HYPED_2019_SENDERINTERFACE_HPP

#include "utils/io/can.hpp"

namespace hyped
{

namespace motor_control
{

class SenderInterface
{
  public:
    virtual void pushSdoMessageToQueue(utils::io::can::Frame &message) = 0;
    virtual void registerController() = 0;
};

} // namespace motor_control
} // namespace hyped

#endif // HYPED_2019_SENDERINTERFACE_HPP
