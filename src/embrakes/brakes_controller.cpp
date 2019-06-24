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
      state_(kNotReadyToSwitchOn),
      node_id_(id),
      can_(Can::getInstance()),
      messageTimestamp(0),
      critical_failure_(false)
{
  isSending = false;

  sdo_message_.id         = kSdoReceive + node_id_;
  sdo_message_.extended   = false;
  sdo_message_.len        = 8;
}

bool Controller::sendControllerMessage(ControllerMessage message_template)
{
  for (int i = 0; i < message_template.len; i++) {
    sdo_message_.data[i] = message_template.message_data[i];
  }

  sendSdoMessage(sdo_message_);

  log_.DBG1("Brakes", message_template.logger_output, node_id_);

  if (critical_failure_) {
    return true;
  }
  return false;
}

void Controller::registerController()
{
  can_.registerProcessor(this);
}

void Controller::checkState()
{
  // Check Statusword in object dictionary
  if (sendControllerMessage(checkStateMsg_[0])) return;
}

void Controller::healthCheck()
{
  // Check warning status & Check error status
  for (int i = 0; i < 2; i++) {
    if (sendControllerMessage(healthCheckMsgs[i])) return;
  }
}

void Controller::sendSdoMessage(utils::io::can::Frame& message)
{
  log_.INFO("MOTOR", "Sending Message");
  can_.send(message);
  isSending = true;

  timer.start();
  messageTimestamp = timer.getTimeMicros();

  while (isSending) {
    if ((timer.getTimeMicros() - messageTimestamp) > TIMEOUT) {
      // TODO(Iain): Test the latency and set the TIMEOUT to a reasonable value.
      log_.ERR("MOTOR", "Sender timeout reached");
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

void Controller::processEmergencyMessage(utils::io::can::Frame& message)
{
  log_.ERR("Brakes", "Controller %d: CAN Emergency", node_id_);
  throwCriticalFailure();
  uint8_t index_1   = message.data[0];
  uint8_t index_2   = message.data[1];

  if (index_2 == 0x00) {
    log_.ERR("Brakes", "Controller %d: No emergency/error", node_id_);
  }
  log_.DBG1("Brakes", "index 1: %d, index 2: %d", index_1, index_2);
}

void Controller::processErrorMessage(uint16_t error_message) {
  switch (error_message) {
    case 0x1000:
      log_.ERR("Brakes", "Controller %d error: Unspecified error", node_id_);
      break;
    case 0x2220:
      log_.ERR("Brakes", "Controller %d error: Overcurrent error", node_id_);
      break;
  }
}

void Controller::processSdoMessage(utils::io::can::Frame& message)
{
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
      uint16_t error_message = (message.data[5] << 8) | message.data[4];
      throwCriticalFailure();
      processErrorMessage(error_message);
    }
    return;
  }

  /* Process Statusword checks
   * xxxx xxxx x0xx 0000: Not ready to switch on
   * xxxx xxxx x1xx 0000: Switch on disabled
   * xxxx xxxx x01x 0001: Ready to switch on
   * xxxx xxxx x01x 0011: Switched on
   * xxxx xxxx x01x 0111: Operation enabled
   * xxxx xxxx x00x 0111: Quick stop active
   * xxxx xxxx x0xx 1111: Fault reaction active
   * xxxx xxxx x0xx 1000: Fault
   */
  if (index_1 == 0x41 && index_2 == 0x60 && sub_index == 0x00) {
    uint8_t status = message.data[4];
    switch (status) {
    case 0x00:
      state_ = kNotReadyToSwitchOn;
      log_.DBG1("Brakes", "Controller %d state: Not ready to switch on", node_id_);
      break;
    case 0x40:
      state_ = kSwitchOnDisabled;
      log_.DBG1("Brakes", "Controller %d state: Switch on disabled", node_id_);
      break;
    case 0x21:
      state_ = kReadyToSwitchOn;
      log_.DBG1("Brakes", "Controller %d state: Ready to switch on", node_id_);
      break;
    case 0x23:
      state_ = kSwitchedOn;
      log_.DBG1("Brakes", "Controller %d state: Switched on", node_id_);
      break;
    case 0x27:
      state_ = kOperationEnabled;
      log_.DBG1("Brakes", "Controller %d state: Operation enabled", node_id_);
      break;
    case 0x07:
      state_ = kQuickStopActive;
      log_.DBG1("Brakes", "Controller %d state: Quick stop active", node_id_);
      break;
    case 0x0F:
      state_ = kFaultReactionActive;
      log_.DBG1("Brakes", "Controller %d state: Fault reaction active", node_id_);
      break;
    case 0x08:
      state_ = kFault;
      log_.DBG1("Brakes", "Controller %d state: Fault", node_id_);
      break;
    default:
      log_.DBG1("Brakes", "Controller %d state: State not recognised", node_id_);
    }
    return;
  }

  // Process configuration messages
  if (index_1 == 0x33 && index_2 == 0x20 && sub_index == 0x00) {
    log_.DBG1("Brakes", "Controller %d: Motor poles configured", node_id_);
    return;
  }
  if (index_1 == 0x40 && index_2 == 0x20 && sub_index == 0x01) {
    log_.DBG1("Brakes", "Controller %d: Feedback type configured", node_id_);
    return;
  }
  if (index_1 == 0x40 && index_2 == 0x20 && sub_index == 0x02) {
    log_.DBG1("Brakes", "Controller %d: Motor phase offset configured", node_id_);
    return;
  }
  if (index_1 == 0x40 && index_2 == 0x20 && sub_index == 0x08) {
    log_.DBG1("Brakes", "Controller %d: Motor phase offset compensation configured", node_id_);
    return;
  }
  if (index_1 == 0x54 && index_2 == 0x20 && sub_index == 0x00) {
    log_.DBG1("Brakes", "Controller %d: Over voltage limit configured", node_id_);
    return;
  }
  if (index_1 == 0x55 && index_2 == 0x20 && sub_index == 0x03) {
    log_.DBG1("Brakes", "Controller %d: Under voltage minimum configured", node_id_);
    return;
  }
  if (index_1 == 0x55 && index_2 == 0x20 && sub_index == 0x01) {
    log_.DBG1("Brakes", "Controller %d: Under voltage limit configured", node_id_);
    return;
  }
  if (index_1 == 0x57 && index_2 == 0x20 && sub_index == 0x01) {
    log_.DBG1("Brakes", "Controller %d: Temperature sensor configured", node_id_);
    return;
  }
  if (index_1 == 0x75 && index_2 == 0x60 && sub_index == 0x00) {
    log_.DBG1("Brakes", "Controller %d: Motor rated current configured", node_id_);
    return;
  }
  if (index_1 == 0x76 && index_2 == 0x60 && sub_index == 0x00) {
    log_.DBG1("Brakes", "Controller %d: Motor rated torque configured", node_id_);
    return;
  }
  if (index_1 == 0xF6 && index_2 == 0x60 && sub_index == 0x01) {
    log_.DBG1("Brakes", "Controller %d: Current control torque P gain configured", node_id_);
    return;
  }
  if (index_1 == 0xF6 && index_2 == 0x60 && sub_index == 0x02) {
    log_.DBG1("Brakes", "Controller %d: Current control torque I gain configured", node_id_);
    return;
  }
  if (index_1 == 0xF6 && index_2 == 0x60 && sub_index == 0x03) {
    log_.DBG1("Brakes", "Controller %d: Current control flux P gain configured", node_id_);
    return;
  }
  if (index_1 == 0xF6 && index_2 == 0x60 && sub_index == 0x04) {
    log_.DBG1("Brakes", "Controller %d: Current control flux I gain configured", node_id_);
    return;
  }
  if (index_1 == 0xF6 && index_2 == 0x60 && sub_index == 0x05) {
    log_.DBG1("Brakes", "Controller %d: Current control ramp configured", node_id_);
    return;
  }
  if (index_1 == 0x50 && index_2 == 0x20 && sub_index == 0x00) {
    log_.DBG1("Brakes", "Controller %d: Maximum current limit configured", node_id_);
    return;
  }
  if (index_1 == 0x51 && index_2 == 0x20 && sub_index == 0x00) {
    log_.DBG1("Brakes", "Controller %d: Secondary current protection configured", node_id_);
    return;
  }
  if (index_1 == 0x52 && index_2 == 0x20 && sub_index == 0x01) {
    log_.DBG1("Brakes", "Controller %d: Maximum RPM configured", node_id_);
    return;
  }

  // Controlword updates
  if (index_1 == 0x40 && index_2 == 0x60 && sub_index == 0x00) {
    log_.DBG1("Brakes", "Controller %d: Control Word updated", node_id_);
    return;
  }
}

bool Controller::getFailure()
{
  return critical_failure_;
}

ControllerState Controller::getControllerState()
{
  return state_;
}

uint8_t Controller::getNode_id()
{
  return node_id_;
}

void Controller::setFailure(bool failure)
{
  critical_failure_ = failure;
}
}}  // namespace hyped::embrakes