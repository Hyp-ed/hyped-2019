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
#include "types/message.pb.h"
#include "main.hpp"
#include "client.hpp"

namespace hyped {

using client::Client;
using data::Data;
using data::Telemetry;

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
    std::thread recvThread {&Main::recvLoop, this};  // NOLINT (linter thinks semicolon is syntax error...)

    while (true) {
        Telemetry telem_data = data_.getTelemetryData();
        log_.DBG2("Telemetry", "SHARED launch_command: %s", telem_data.launch_command ? "true" : "false"); // NOLINT
        log_.DBG2("Telemetry", "SHARED run_length: %f", telem_data.run_length);

        telemetry_data::TestMessage msg;

        msg.set_command(telemetry_data::TestMessage::VELOCITY);
        msg.set_data(222);
        client_.sendData(msg);

        msg.set_command(telemetry_data::TestMessage::ACCELERATION);
        msg.set_data(333);
        client_.sendData(msg);

        msg.set_command(telemetry_data::TestMessage::BRAKE_TEMP);
        msg.set_data(777);
        client_.sendData(msg);

        msg.set_command(telemetry_data::TestMessage::VELOCITY);
        msg.set_data(333);
        client_.sendData(msg);

        msg.set_command(telemetry_data::TestMessage::ACCELERATION);
        msg.set_data(444);
        client_.sendData(msg);

        msg.set_command(telemetry_data::TestMessage::BRAKE_TEMP);
        msg.set_data(888);
        client_.sendData(msg);

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));  // breaks GUI, must fix
    }

    recvThread.join();
}

void Main::recvLoop()
{
    telemetry_data::ServerToClient msg;
    // not sure whether to put this in or ouside of loop
    Telemetry shared_telem_data = data_.getTelemetryData();

    while (true) {
        msg = client.receiveData();

        switch (msg.command()) {
            case telemetry_data::ServerToClient::LAUNCH:
                log_.DBG1("Telemetry", "FROM SERVER: LAUNCH");
                shared_telem_data.launch_command = true;
                break;
            case telemetry_data::ServerToClient::RUN_LENGTH:
                log_.DBG1("Telemetry", "FROM SERVER: RUN_LENGTH %f", msg.run_length());
                shared_telem_data.run_length = msg.run_length();
                break;
            default:
                log_.ERR("Telemetry", "Received unrecognized input from server");
        }

        data_.setTelemetryData(shared_telem_data);
    }
}

}  // namespace telemetry
}  // namespace hyped
