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

stepper_position_LSB = 0x0;
stepper_position_MSB = 0x0;
stepper_period = 0xA;

message_to_send.id       = kSdoReceive + node_id_;
message_to_send.extended = false;
message_to_send.len      = 8;    
}

void Stepper::registerStepper()
{
  can_.registerProcessor(this);
}

void Stepper::ProcessNewData(Frame &message)
{
  uint32_t id = message.id;
  if(id == kSdoTransmit + node_id_) {
    uint8_t isEnabled = message.data[0];
    uint16_t stepper_position = (message.data[2] << 8)| message.data[1];
    uint8_t isHome = message.data[3];
    uint16_t current_average = (message.data[5] << 8)| message.data[4];
    uint8_t can_id = message.data[6];

    checkHome(isHome);
    
  } else{
    log_.ERR("Brakes", "Stepper %d: CAN message not recognised", node_id_);
  }
}

bool Stepper::hasId(uint32_t id, bool extended)
{
  for (uint32_t canID : canIds) {
    if (canID + node_id_ == id) {
      return true;
    }
  }
  return false;
}

void Stepper::checkHome(uint8_t button) 
{
  // make sure current brake is noted as retracted or not (based on the message received) in the data struct
  uint8_t brk_id = node_id_ - 20;
  if(button && !em_brakes_data_.brakes_retracted[brk_id]){
    em_brakes_data_.brakes_retracted[brk_id] = true;
    data_.setEmergencyBrakesData(em_brakes_data_);
  } else if(!button && em_brakes_data_.brakes_retracted[brk_id]){
    em_brakes_data_.brakes_retracted[brk_id] = false;
  }
}

void Stepper::sendRetract(){
  log_.INFO("Brakes", "Sending a retract message");
  message_to_send.data[0] = 0x1;
  message_to_send.data[4] = stepper_period;
// what would the target position be?? can we just hard code it based on experimentation
  can_.send(message_to_send);
  
}

void Stepper::sendClamp(){
  log_.INFO("Brakes", "Calmping the brakes");
  message_to_send.data[0] = 0x0;
  message_to_send.data[4] = stepper_period;
// what would the target position be? do we need to specify it at all
  can_.send(message_to_send);
}

}}  // namespace hyped::embrakes