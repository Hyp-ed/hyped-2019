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

#ifndef TELEMETRY_CLIENT_HPP_
#define TELEMETRY_CLIENT_HPP_

#include <google/protobuf/io/zero_copy_stream.h>
#include <string>
#include "types/message.pb.h"
#include "utils/logger.hpp"

namespace hyped {

using utils::Logger;
using google::protobuf::io::ZeroCopyInputStream;

namespace client {

constexpr auto kPort = "9090";
// constexpr auto kServerIP = "localhost";
constexpr auto kServerIP = "192.168.1.50";

class Client {
    public:
        explicit Client(Logger& log);
        ~Client();
        bool sendData(protoTypes::TestMessage message);
        bool receiveData();

    private:
        int sockfd_;
        Logger& log_;
        // socket_stream_ is member var bc need to keep reading from same stream
        ZeroCopyInputStream* socket_stream_;
};

}  // namespace client
}  // namespace hyped

#endif  // TELEMETRY_CLIENT_HPP_
