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

#include "embrakes/brakes_controller.hpp"

namespace hyped {
namespace embrakes {

Controller::Controller(Logger& log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      brakes_data_(data_.getEmergencyBrakesData()),
      state_(kClamped),
      node_id_(id),
      critical_failure_(false),
      can_(Can::getInstance()),
      messageTimestamp(0)
{
  isSending = false;
  can_.start();

  sdo_message_.id         = kSdoReceive + node_id_;
  sdo_message_.extended   = false;
  sdo_message_.len        = 8;
}

void Controller::registerController()
{
  can_.registerProcessor(this);
}

void Controller::sendSdoMessage(utils::io::can::Frame& message)
{
  log_.INFO("BRAKES", "Sending Message");
  can_.send(message);
  isSending = true;

  timer.start();
  messageTimestamp = timer.getTimeMicros();

  while (isSending) {
    if ((timer.getTimeMicros() - messageTimestamp) > TIMEOUT) {
      log_.ERR("MOTOR", "Sender timeout reached");
      throwCriticalFailure();
    }
  }
}

void Controller::throwCriticalFailure()
{
  critical_failure_ = true;
  brakes_data_ = data_.getEmergencyBrakesData();
  brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
  data_.setEmergencyBrakesData(brakes_data_);
}

void Controller::sendData(uint8_t* message_data)
{
  int size_of_data = sizeof(message_data)/sizeof(message_data[0]);
  for(int i = 0; i < size_of_data; i++){
    sdo_message_.data[i] = message_data[i];
  }
  sendSdoMessage(sdo_message_);
}

void Controller::processNewData(utils::io::can::Frame& message)
{
  isSending = false;
  uint8_t index_1   = message.data[1];
  uint8_t index_2   = message.data[2];
  uint8_t sub_index = message.data[3];

  // Process warning message
  if (index_1 == 0x27 && index_2 == 0x20 && sub_index == 0x00) {
    if (message.data[4] != 0 && message.data[5] != 0) {
      throwCriticalFailure();
      uint8_t warning_message = message.data[4];
      switch (warning_message) {
        case 0x01:
          log_.ERR("Brakes", "Controller %d warning: Controller temperature exceeded");
          break;
        default:
          log_.ERR("Brakes", "Controller %d warning: Warning code %d", node_id_, warning_message);
      }
    }
    return;
  }

  // Process error message
  if (index_1 == 0x3F && index_2 == 0x60 && sub_index == 0x00) {
    if (message.data[4] != 0 && message.data[5] != 0) {
      throwCriticalFailure();
    }
    return;
  }
}

ControllerState Controller::getControllerState()
{
  return state_;
}

bool Controller::hasId(uint32_t id, bool extended)
{
  for (uint32_t cobId : canIds) {
    if (cobId + node_id_ == id) {
      return true;
    }
  }
  return false;
}

}}  // namespace hyped::embrakes