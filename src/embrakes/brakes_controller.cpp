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
      critical_failure_(false),
      actual_velocity_(0),
      actual_torque_(0),
      motor_temperature_(0),
      controller_temperature_(0),
      sender(this, node_id_, log_)
{
  sdo_message_.id         = kSdoReceive + node_id_;
  sdo_message_.extended   = false;
  sdo_message_.len        = 8;

  nmt_message_.id         = kNmtReceive;
  nmt_message_.extended   = false;
  nmt_message_.len        = 2;

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
  sender.registerController();
}

void Controller::configure()
{
  log_.INFO("Brakes", "Controller %d: Configuring...", node_id_);
  for (int i = 0; i < 16; i++) {
    if (sendControllerMessage(configMsgs_[i])) return;
  }
  log_.INFO("Brakes", "Controller %d: Configured.", node_id_);
}

void Controller::enterOperational()
{
  // Send NMT Operational message to transition from state 0 (Not ready to switch on)
  // to state 1 (Switch on disabled)
  nmt_message_.data[0] = kNmtOperational;
  nmt_message_.data[1] = node_id_;

  log_.INFO("Brakes", "Controller %d: Sending NMT Operational command", node_id_);
  sender.sendMessage(nmt_message_);
  // leave time for the controller to enter NMT Operational
  Thread::sleep(100);

  // enables velocity mode
  if (sendControllerMessage(enterOpMsgs_[0])) return;

  // apply break
  if (sendControllerMessage(enterOpMsgs_[1])) return;

  // send shutdown message to transition to Ready to Switch On state
  for (int i = 0; i < 8; i++) {
    sdo_message_.data[i] = enterOpMsgs_[2].message_data[i];
  }
  log_.DBG1("Brakes", enterOpMsgs_[2].logger_output, node_id_);
  requestStateTransition(sdo_message_, kReadyToSwitchOn);

  // Send enter operational message to transition to the Operation Enabled state
  for (int i = 0; i < 8; i++) {
    sdo_message_.data[i] = enterOpMsgs_[3].message_data[i];
  }
  log_.DBG1("Brakes", enterOpMsgs_[3].logger_output, node_id_);
  requestStateTransition(sdo_message_, kOperationEnabled);
}

void Controller::enterPreOperational()
{
  checkState();
  if (state_ != kReadyToSwitchOn) {
    // send shutdown command
    if (sendControllerMessage(enterPreOpMsg_[0])) return;
  }
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
  if (!sender.sendMessage(message)) {
    log_.ERR("Brakes", "Controller %d: No response from controller", node_id_);
    throwCriticalFailure();
  }
}

void Controller::throwCriticalFailure()
{
  critical_failure_ = true;
  brakes_data_ = data_.getEmergencyBrakesData();
  brakes_data_.module_status = data::ModuleStatus::kCriticalFailure;
  data_.setEmergencyBrakesData(brakes_data_);
}

void Controller::requestStateTransition(utils::io::can::Frame& message, ControllerState state)
{
  uint8_t state_count;
  // Wait for max of 3 seconds, checking if the state has changed every second
  // If it hasn't changed by the end then throw critical failure.
  for (state_count = 0; state_count < 3; state_count++) {
    sender.sendMessage(message);
    Thread::sleep(1000);
    checkState();
    if (state_ == state) {
      return;
    }
  }
  if (state_ != state) {
    throwCriticalFailure();
    log_.ERR("Brakes", "Controller %d, Could not transition to state %d", node_id_, state);
    return;
  }
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

void Controller::processNmtMessage(utils::io::can::Frame& message)
{
  int8_t nmt_state = message.data[0];
  switch (nmt_state) {
    case 0x00:
      log_.INFO("Brakes", "Controller %d NMT State: Bootup", node_id_);
      break;
    case 0x04:
      log_.INFO("Brakes", "Controller %d NMT State: Stopped", node_id_);
      break;
    case 0x05:
      log_.INFO("Brakes", "Controller %d NMT State: Operational", node_id_);
      break;
    case 0x7F:
      log_.INFO("Brakes", "Controller %d NMT State: Pre-Operational", node_id_);
      break;
    default:
      log_.ERR("Brakes", "Controller %d NMT State: Not Recognised", node_id_);
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