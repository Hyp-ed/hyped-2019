/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: March 2019
 * Description:
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

#ifndef TELEMETRY_MAIN_HPP_
#define TELEMETRY_MAIN_HPP_

#include "telemetry/telemetrydata/message.pb.h"
#include "client.hpp"
#include "data/data.hpp"
#include "utils/concurrent/thread.hpp"

namespace hyped {

using client::Client;
using utils::concurrent::Thread;
using utils::Logger;
using data::Data;

namespace telemetry {

class Main: public Thread {
    public:
        Main(uint8_t id, Logger& log);
        void run() override;

    private:
        Client client_;
        void sendLoop();
        void recvLoop();
        void packNavigationData(telemetry_data::ClientToServer& msg);
        void packStateMachineData(telemetry_data::ClientToServer& msg);
        void packMotorsData(telemetry_data::ClientToServer& msg);
        Data& data_;
        data::Navigation        nav_data_;
        data::StateMachine      sm_data_;
        data::Motors            motor_data_;
        data::Batteries         batteries_data_;
        data::Sensors           sensors_data_;
        data::EmergencyBrakes   emergency_brakes_data_;
};

}  // namespace telemetry
}  // namespace hyped

#endif  // TELEMETRY_MAIN_HPP_
