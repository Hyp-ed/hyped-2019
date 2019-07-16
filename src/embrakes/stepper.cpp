/*
* Author: Kornelija Sukyte
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

#include "embrakes/stepper.hpp"

namespace hyped {
namespace embrakes {

Stepper::Stepper(Logger& log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      node_id_(id),
      can_(Can::getInstance()),
      em_brakes_data_(data_.getEmergencyBrakesData()),
      tlm_data_(data_.getTelemetryData()),
      sm_data_(data_.getStateMachineData())
{
can_.start();

stepper_position_LSB = 0x00;
stepper_position_MSB = 0x00;
stepper_period       = 0x28;
isEnabled            = 0x00; // is true (or set to anything above 0) if brakes to be retracted
isHome               = 0x01; // is 0 if button is pressed (if brakes are retracted)

message_to_send.id       = node_id_;
message_to_send.extended = false;
message_to_send.len      = 8;    
}

void Stepper::registerStepper()
{
  can_.registerProcessor(this);
}

void Stepper::processNewData(utils::io::can::Frame &message)
{
  uint32_t id = message.id;
  uint8_t ex = message.extended;
  if(hasId(id, ex)) {
    uint8_t isEnabled = message.data[0];
    uint8_t stepper_position_LSB = message.data[1];
    uint8_t stepper_position_MSB = message.data[2];
    uint8_t isHome = message.data[3];
    uint8_t can_id = message.data[6];

    checkHome(isHome);
    
  } else {
    log_.ERR("Brakes", "Stepper %d, %i: CAN message not recognised", node_id_);
  }
}

bool Stepper::hasId(uint32_t id, bool extended)
{
  if (node_id_ == id) {
    return true;
  }
  return false;
}

void Stepper::checkHome(uint8_t button) 
{
  // make sure current brake is noted as retracted or not (based on the message received) in the data struct
  uint8_t brk_id = node_id_ - 20;
  if(!button && !em_brakes_data_.brakes_retracted[brk_id]){
    em_brakes_data_.brakes_retracted[brk_id] = true;
    data_.setEmergencyBrakesData(em_brakes_data_);
  } else if(button && em_brakes_data_.brakes_retracted[brk_id]){
    em_brakes_data_.brakes_retracted[brk_id] = false;
    data_.setEmergencyBrakesData(em_brakes_data_);
  }
}

void Stepper::sendRetract(uint8_t MSB, uint8_t LSB, uint8_t period){
  log_.INFO("Brakes", "Sending a retract message to brake %i", node_id_-20);
  message_to_send.data[0] = 0x01;
  message_to_send.data[1] = MSB;
  message_to_send.data[2] = LSB;
  message_to_send.data[3] = period;
  
  can_.send(message_to_send);
  
}

void Stepper::sendClamp(uint8_t enabled, uint8_t period){

  message_to_send.data[0] = enabled;
  message_to_send.data[1] = 0x13;
  message_to_send.data[2] = 0x88;
  message_to_send.data[3] = period;

  can_.send(message_to_send);

}

}}  // namespace hyped::embrakes