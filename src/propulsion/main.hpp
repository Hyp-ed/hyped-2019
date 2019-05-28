/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date: 1.4.2019
 * Description: Main entrypoint to motor control module
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

#ifndef PROPULSION_MAIN_HPP_
#define PROPULSION_MAIN_HPP_

#include "utils/concurrent/thread.hpp"
#include "utils/concurrent/barrier.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "./can/can_sender.hpp"
#include "data/data.hpp"

#include "state_processor.hpp"

namespace hyped
{
using data::Data;
using data::State;
using data::ModuleStatus;
using data::Motors;
using utils::Logger;
using utils::System;
using utils::concurrent::Thread;

namespace motor_control
{
class Main : public Thread
{
    public:
        Main(uint8_t id, Logger &log);

    /**
     * @brief {This function is the entrypoint to the propulsion module and reacts to the certain states}
    * */
    void run() override;

  private:
    bool isRunning;
    Logger &log_;
    StateProcessor *stateProcessor;
    State currentState;
};

}  // namespace motor_control
}  // namespace hyped

#endif  // PROPULSION_MAIN_HPP_
