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

#ifndef EMBRAKES_STEPPER_HPP_
#define EMBRAKES_STEPPER_HPP_

#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "utils/io/can.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::io::can::Frame;
using utils::Logger;
using utils::io::Can;
using utils::io::CanProccesor;

namespace embrakes {

// Types of CANopen messages, these are used for CAN ID's

constexpr uint32_t kSdoReceive = 0x600;
constexpr uint32_t kSdoTransmit = 0x580;

constexpr uint32_t canIds[13] {0x600, 0x580};

class Stepper :public CanProccesor {
  public:
  /**
   * @brief Construct a new Stepper object
   * @param log, node id
   */
  Stepper(Logger& log, uint8_t id);

  /**
     * @brief { Registers stepper to process incoming CAN messages }
     */
  void registerStepper();

  /**
     * @brief { This function processes incoming CAN messages }
     */
  void processNewData(utils::io::can::Frame &message) override;

  /**
     * @brief { Returns true if the CAN message is ment for this CAN node }
     */
  bool hasId(uint32_t id, bool extended) override;

  /**
   * @brief {checks if brake's button is pressed, notes change in the data struct}
   */
  void checkHome(uint8_t button);

  /**
   * @brief sends retract message
   */
  void sendRetract(uint8_t LSB, uint8_t MSB, uint8_t period);

  /**
   * @brief sends clamp message
   */
  void sendClamp();

  private:

    utils::Logger&        log_;
    data::Data&           data_;
    uint8_t               node_id_;
    Can&                  can_;
    data::EmergencyBrakes em_brakes_data_;
    data::Telemetry       tlm_data_;
    data::StateMachine    sm_data_;
    Frame                 message_to_send;
    uint8_t               stepper_position_LSB;
    uint8_t               stepper_position_MSB;
    uint8_t               stepper_period;
    uint8_t               isEnabled;
    uint8_t               isHome;

};

}}  // namespace hyped::embrakes

#endif  // EMBRAKES_STEPPER_HPP_