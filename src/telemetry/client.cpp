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

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "client.hpp"

namespace hyped {
namespace client {

Client::Client(Logger& log)
    : log_(log)
{
    struct addrinfo hints;
    struct addrinfo* server_info;  // contains possible addresses to connect to according to hints

    // set up criteria for type of address we want to connect to
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // get possible addresses we can connect to
    int return_val;
    if ((return_val = getaddrinfo(server_ip, port, &hints, &server_info)) != 0) {
        log_.ERR("Telemetry", "%s", gai_strerror(return_val));
        // exit(1);
        // probably throw exception here or something
    }

    // get a socket file descriptor
    sockfd_ = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (sockfd_ == -1) {
        log_.ERR("Telemetry", "%s", strerror(errno));
        // exit(2);
        // probably throw exception here or something
    }

    // connect socket to server
    if (connect(sockfd_, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        close(sockfd_);
        log_.ERR("Telemetry", "%s", strerror(errno));
        // exit(3);
        // probably throw exception here or something
    }

    log_.INFO("Telemetry", "Connected to server");

    socket_stream_ = new google::protobuf::io::FileInputStream(sockfd_);
}

Client::~Client()
{
    delete socket_stream_;
    close(sockfd_);
}

// message has to be terminated by newline bc we read messages on server using in.readLine()
bool Client::sendData(protoTypes::TestMessage message)
{
    using namespace google::protobuf::util;

    if (!SerializeDelimitedToFileDescriptor(message, sockfd_)) {
        log_.ERR("Telemetry", "SerializeDelimitedToFileDescriptor didn't work");
        return false;
    }

    return true;
}

bool Client::receiveData()
{
    using namespace google::protobuf::util;

    protoTypes::TestMessage messageFromServer;
    if (!ParseDelimitedFromZeroCopyStream(&messageFromServer, socket_stream_, NULL)) {
        log_.ERR("Telemetry", "ParseDelimitedFromZeroCopyStream didn't work");
        return false;
    }

    switch (messageFromServer.command()) {
        case protoTypes::TestMessage::FINISH:
            log_.DBG1("Telemetry", "FROM SERVER: FINISH");
            break;
        case protoTypes::TestMessage::EM_STOP:
            log_.DBG1("Telemetry", "FROM SERVER: EM_STOP");
            break;
        default:
            log_.ERR("Telemetry", "UNRECOGNIZED INPUT FROM SERVER");
            break;
    }

    return true;
}

}  // namespace client
}  // namespace hyped
