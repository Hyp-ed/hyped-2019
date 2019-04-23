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
#include "telemetry/main.hpp"
#include "telemetry/sendloop.hpp"
#include "telemetry/utils.hpp"

namespace hyped {

using data::ModuleStatus;

namespace telemetry {

Main::Main(uint8_t id, Logger& log)
    : Thread {id, log},
      client_ {log},
      data_ {Data::getInstance()}
{
    log_.DBG("Telemetry", "Telemetry Main thread object created");
}

void Main::run()
{
    log_.DBG("Telemetry", "Telemetry Main thread started");

    if (!client_.connect()) {
        // idk throw exception or something
        log_.ERR("Telemetry", "ERROR CONNECTING TO SERVER");
    }

    // syntax explanation so I don't forget: thread constructor expects pointer to member function,
    //                                       also needs 'this' as object to call member function on
    std::thread recv_thread {&Main::recvLoop, this};  // NOLINT (linter thinks semicolon is syntax error...)
    Thread* sendloop_thread = new SendLoop(log_, this);
    sendloop_thread->start();

    sendloop_thread->join();
    delete sendloop_thread;
    recv_thread.join();
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
                log_.ERR("Telemetry", "Unrecognized input from server, ENTERING CRITICAL FAILURE");
                telem_data_struct.module_status = ModuleStatus::kCriticalFailure;
                break;
        }

        data_.setTelemetryData(telem_data_struct);
    }
}

}  // namespace telemetry
}  // namespace hyped
