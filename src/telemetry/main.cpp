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

#include <thread>
#include "telemetry/telemetrydata/message.pb.h"
#include "main.hpp"

namespace hyped {

using client::Client;
using data::ModuleStatus;
using data::Data;

namespace telemetry {

Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      client_ {log},
      data_ {Data::getInstance()}
{
    log_.DBG("Telemetry", "Telemetry thread started");
}

void Main::run()
{
    if (!client_.connect()) {
        // idk throw exception or something
        log_.ERR("Telemetry", "ERROR CONNECTING TO SERVER");
    }

    // syntax explanation so I don't forget: thread constructor expects pointer to member function,
    //                                       also needs 'this' as object to call member function on
    std::thread recv_thread {&Main::recvLoop, this};  // NOLINT (linter thinks semicolon is syntax error...)
    std::thread send_thread {&Main::sendLoop, this};  // NOLINT (linter thinks semicolon is syntax error...)

    recv_thread.join();
    send_thread.join();
}

void Main::sendLoop()
{
    telemetry_data::ClientToServer msg;

    while (true) {
        nav_data_               = data_.getNavigationData();
        sm_data_                = data_.getStateMachineData();
        motor_data_             = data_.getMotorData();
        batteries_data_         = data_.getBatteriesData();
        sensors_data_           = data_.getSensorsData();
        emergency_brakes_data_  = data_.getEmergencyBrakesData();

        telemetry_data::ClientToServer::Navigation* navigation_msg = msg.mutable_navigation();
        navigation_msg->set_distance(nav_data_.distance);
        navigation_msg->set_velocity(nav_data_.velocity);
        navigation_msg->set_acceleration(nav_data_.acceleration);

        client_.sendData(msg);
        msg.Clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Main::recvLoop()
{
    telemetry_data::ServerToClient msg;
    // not sure whether to put this in or ouside of loop
    data::Telemetry telem_data_struct = data_.getTelemetryData();

    while (true) {
        msg = client_.receiveData();

        switch (msg.command()) {
            case telemetry_data::ServerToClient::ACK:
                log_.DBG1("Telemetry", "FROM SERVER: ACK");
                break;
            case telemetry_data::ServerToClient::STOP:
                log_.DBG1("Telemetry", "FROM SERVER: STOP");
                telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
                break;
            case telemetry_data::ServerToClient::LAUNCH:
                log_.DBG1("Telemetry", "FROM SERVER: LAUNCH");
                telem_data_struct.launch_command = true;
                break;
            case telemetry_data::ServerToClient::RESET:
                log_.DBG1("Telemetry", "FROM SERVER: RESET");
                telem_data_struct.reset_command = true;
                break;
            case telemetry_data::ServerToClient::RUN_LENGTH:
                log_.DBG1("Telemetry", "FROM SERVER: RUN_LENGTH %f", msg.run_length());
                telem_data_struct.run_length = msg.run_length();
                break;
            case telemetry_data::ServerToClient::SERVICE_PROPULSION:
                log_.DBG1("Telemetry", "FROM SERVER: SERVICE_PROPULSION %s", msg.service_propulsion() ? "true" : "false");  // NOLINT
                telem_data_struct.service_propulsion_go = msg.service_propulsion();
                break;
            default:
                log_.ERR("Telemetry", "Unrecognized input from server, entering critical failure");
                telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
                break;
        }

        data_.setTelemetryData(telem_data_struct);
    }
}

}  // namespace telemetry
}  // namespace hyped
