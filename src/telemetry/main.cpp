/*
 * Authors:
 * Organisation: HYPED
 * Date:
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
#include "main.hpp"
#include "client.hpp"
#include "types/message.pb.h"
#include <thread>

namespace hyped {

using hyped::client::Client;

namespace telemetry {

Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      client{}
{
    log_.INFO("Telemetry", "Logger constructor was called");
}

void Main::run() {
    std::thread recvThread {recvLoop, std::ref(client)};

    while (true) {
        protoTypes::TestMessage msg;

        msg.set_command(protoTypes::TestMessage::VELOCITY);
        msg.set_data(222);
        client.sendData(msg);

        msg.set_command(protoTypes::TestMessage::ACCELERATION);
        msg.set_data(333);
        client.sendData(msg);

        msg.set_command(protoTypes::TestMessage::BRAKE_TEMP);
        msg.set_data(777);
        client.sendData(msg);

        msg.set_command(protoTypes::TestMessage::VELOCITY);
        msg.set_data(333);
        client.sendData(msg);

        msg.set_command(protoTypes::TestMessage::ACCELERATION);
        msg.set_data(444);
        client.sendData(msg);

        msg.set_command(protoTypes::TestMessage::BRAKE_TEMP);
        msg.set_data(888);
        client.sendData(msg);
    }

    recvThread.join();
}

void recvLoop(Client& c) {
    while (true) {
        c.receiveData();
    }
}

}  // namespace telemetry
}  // namespace hyped
