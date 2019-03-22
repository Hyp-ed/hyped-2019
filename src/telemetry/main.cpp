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
#include "client.hpp"
#include <thread>

namespace hyped {
namespace communications {

}  // namespace communications
}  // namespace hyped

using hyped::client::Client;

int main(void) {
    Client client {};

    // std::thread recvThread {&Client::receiveData, client};

    while (true) {
        client.sendData("hello from client\n");
        // client.receiveData();
    }

    // recvThread.join();
    return 0;
}
