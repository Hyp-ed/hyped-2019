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
      actual_velocity_(0),
      sender(log_, node_id_)
{
  // TODO(Iain): Check if this is still valid:
  sdo_message_.id         = kSdoReceive + node_id_;
  sdo_message_.extended   = false;
  sdo_message_.len        = 8;

  nmt_message_.id         = kNmtReceive;
  nmt_message_.extended   = false;
  nmt_message_.len        = 8;
}

void Controller::registerController()
{
  sender.registerController();
}

void Controller::configure()
{
  log_.INFO("MOTOR", "Controller %d: Configuring...", node_id_);

  // TODO(Iain): configure motor poles
  for (int i = 0; i < sdo_message_.len; i++) {
    sdo_message_.data[i] = sample_message_data_[i];
  }

  log_.DBG1("MOTOR", "Controller %d: Configuring motor poles", node_id_);

  sender.sendMessage(sdo_message_);

  if (critical_failure_) return;
}

void Controller::enterOperational()
{
}

void Controller::enterPreOperational()
{
}

void Controller::checkState()
{
}

void Controller::sendTargetVelocity(int32_t target_velocity)
{
}

void Controller::updateActualVelocity()
{
}

void Controller::quickStop()
{
}

void Controller::healthCheck()
{
}

int32_t Controller::getVelocity()
{
  return actual_velocity_;
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

}}  // namespace hyped::motor_control
