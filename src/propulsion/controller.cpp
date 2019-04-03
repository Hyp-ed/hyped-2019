/*
 * Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for fake IMUs
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

#include "propulsion/controller.hpp"

namespace hyped {
namespace motor_control {

Controller::Controller(Logger& log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      motor_data_(data_.getMotorData()),
      state_(kNotReadyToSwitchOn),
      node_id_(id),
      critical_failure_(false),
      sdo_frame_recieved_(false),
      actual_velocity_(0),
      actual_torque_(0),
      motor_temperature_(0),
      controller_temperature_(0),
      sender(log_, node_id_)
{
  // TODO(Iain): Check if this is still valid:
  sdo_message_.id         = kSdoReceive + node_id_;
  sdo_message_.extended   = false;
  sdo_message_.len        = 8;

  nmt_message_.id         = kNmtReceive;
  nmt_message_.extended   = false;
  nmt_message_.len        = 8;

  // Initialse arrays of message data:
  FileReader::readFileData(configMsgs_, kConfigMsgFile);
  FileReader::readFileData(enterOpMsgs_, kEnterOpMsgFile);
  FileReader::readFileData(enterPreOpMsg_, kEnterPreOpMsgFile);
  FileReader::readFileData(checkStateMsg_, kCheckStateMsgFile);
  FileReader::readFileData(sendTargetVelMsg, kSendTargetVelMsgFile);
  FileReader::readFileData(sendTargetTorqMsg, kSendTargetTorqMsgFile);
  FileReader::readFileData(updateActualVelMsg, kUpdateActualVelMsgFile);
  FileReader::readFileData(updateActualTorqMsg, kUpdateActualTorqMsgFile);
  FileReader::readFileData(quickStopMsg, kQuickStopMsgFile);
  FileReader::readFileData(healthCheckMsgs, kHealthCheckMsgFile);
  FileReader::readFileData(updateMotorTempMsg, kUpdateMotorTempFile);
  FileReader::readFileData(updateContrTempMsg, kUpdateContrTempFile);
}

bool Controller::hasId(uint32_t id, bool extended)
{
  return true;
}

bool Controller::sendControllerMessage(ControllerMessage message_template)
{
  for (int i = 0; i < message_template.len; i++) {
    sdo_message_.data[i] = message_template.message_data[i];
  }

  sendSdoMessage(sdo_message_);

  log_.DBG1("MOTOR", message_template.logger_output, node_id_);

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
  log_.INFO("MOTOR", "Controller %d: Configuring...", node_id_);
  for (int i = 0; i < 16; i++) {
    if (sendControllerMessage(configMsgs_[i])) return;
  }
  log_.INFO("MOTOR", "Controller %d: Configured.", node_id_);
}

void Controller::enterOperational()
{
  // TODO(Iain): Check that this is still valid:

  // Send NMT Operational message to transition from state 0 (Not ready to switch on)
  // to state 1 (Switch on disabled)
  nmt_message_.data[0] = kNmtOperational;
  nmt_message_.data[1] = node_id_;

  log_.INFO("MOTOR", "Controller %d: Sending NMT Operational command", node_id_);
  sender.sendMessage(nmt_message_);
  // leave time for the controller to enter NMT Operational
  Thread::sleep(100);

  // enables velocity mode
  if (sendControllerMessage(enterOpMsgs_[0])) return;

  // set the velocity to zero
  sendTargetVelocity(0);

  // apply break
  if (sendControllerMessage(enterOpMsgs_[1])) return;

  // send shutdown message to transition to Ready to Switch On state
  for (int i = 0; i < 8; i++) {
    sdo_message_.data[i] = enterOpMsgs_[2].message_data[i];
  }
  log_.DBG1("MOTOR", enterOpMsgs_[2].logger_output, node_id_);
  requestStateTransition(sdo_message_, kReadyToSwitchOn);

  // Send enter operational message to transition to the Operation Enabled state
  for (int i = 0; i < 8; i++) {
    sdo_message_.data[i] = enterOpMsgs_[3].message_data[i];
  }
  log_.DBG1("MOTOR", enterOpMsgs_[3].logger_output, node_id_);
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

void Controller::sendTargetVelocity(int32_t target_velocity)
{
  // Send 32 bit integer in Little Edian bytes
  for (int i = 0; i < 8; i++) {
    sdo_message_.data[i] = sendTargetVelMsg[0].message_data[i];
  }
  sdo_message_.data[4] = target_velocity & 0xFF;
  sdo_message_.data[5] = (target_velocity >> 8) & 0xFF;
  sdo_message_.data[6] = (target_velocity >> 16) & 0xFF;
  sdo_message_.data[7] = (target_velocity >> 24) & 0xFF;

  log_.DBG2("MOTOR", sendTargetVelMsg[0].logger_output, node_id_, target_velocity);
  sender.sendMessage(sdo_message_);
  // TODO(Iain): ^ why is this one can.send instead of sendSdoMessage?
}

void Controller::sendTargetTorque(int16_t target_torque)  // should this be int32_t?
{
  // Send 32 bit integer in Little Edian bytes
  for (int i = 0; i < 8; i++) {
    sdo_message_.data[i] = sendTargetTorqMsg[0].message_data[i];
  }
  sdo_message_.data[4] = target_torque & 0xFF;
  sdo_message_.data[5] = (target_torque >> 8) & 0xFF;

  log_.DBG2("MOTOR", sendTargetTorqMsg[0].logger_output, node_id_, target_torque);
  sendSdoMessage(sdo_message_);
}

void Controller::updateActualVelocity()
{
  // Check actual velocity in object dictionary
  if (sendControllerMessage(updateActualVelMsg[0])) return;
}

void Controller::updateActualTorque()
{
  // Check actual torque in object dictionary
  if (sendControllerMessage(updateActualTorqMsg[0])) return;
}


void Controller::quickStop()
{
  // Send quickStop command
  if (sendControllerMessage(quickStopMsg[0])) return;
}

void Controller::healthCheck()
{
  // Check warning status & Check error status
  for (int i = 0; i < 2; i++) {
    if (sendControllerMessage(healthCheckMsgs[i])) return;
  }
}

void Controller::updateMotorTemp()
{
  // Check motor temp in object dictionary
  if (sendControllerMessage(updateMotorTempMsg[0])) return;
}

void Controller::updateControllerTemp()
{
  // Check controller temp in object dictionary
  if (sendControllerMessage(updateContrTempMsg[0])) return;
}

void Controller::sendSdoMessage(utils::io::can::Frame& message)
{
  sdo_frame_recieved_ = false;
  int8_t send_counter = 0;

  for (send_counter = 0; send_counter < 3; send_counter++) {
    sender.sendMessage(message);
    Thread::yield();
    if (sdo_frame_recieved_) {
      break;
    } else {
      log_.DBG1("MOTOR", "Controller %d: No response. Sending SDO frame again", node_id_);
    }
  }
  // No SDO frame recieved - controller is offline/communication error
  if (!sdo_frame_recieved_) {
    log_.ERR("MOTOR", "Controller %d: No response from controller", node_id_);
    throwCriticalFailure();
  }
}

void Controller::throwCriticalFailure()
{
  critical_failure_ = true;
  motor_data_ = data_.getMotorData();  // necissary?
  motor_data_.module_status = data::ModuleStatus::kCriticalFailure;
  data_.setMotorData(motor_data_);
}

void Controller::requestStateTransition(utils::io::can::Frame& message, ControllerState state)
{
  uint8_t state_count;  // any reason that this is out here?
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
    log_.ERR("MOTOR", "Controller %d, Could not transition to state %d", node_id_, state);
    return;
  }
}

void Controller::processEmergencyMessage(utils::io::can::Frame& message)
{
}

void Controller::processErrorMessage(uint16_t error_message)
{
}

void Controller::processSdoMessage(utils::io::can::Frame& message)
{
}

void Controller::processNmtMessage(utils::io::can::Frame& message)
{
}

void Controller::processNewData(utils::io::can::Frame& message)
{
}

int32_t Controller::getVelocity()
{
  return actual_velocity_;
}

int16_t Controller::getTorque()
{
  return actual_torque_;
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

uint8_t Controller::getMotorTemp()
{
  return motor_temperature_;
}

uint8_t Controller::getControllerTemp()
{
  return controller_temperature_;
}
}}  // namespace hyped::motor_control
