/*
 * Authors : HYPED
 * Organisation: HYPED
 * Date: 3. February 2018
 * Description:
 * This is the main executable for BeagleBone pod node
 *
 *    Copyright 2018 HYPED
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace hyped {
namespace client {

Client::Client() {
    struct addrinfo hints;
    struct addrinfo* server_info; // will contain possible addresses to connect to according to hints

    // set up criteria for type of address we want to connect to
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // get possible addresses we can connect to
    int return_val;
    if ((return_val = getaddrinfo(server_ip, port, &hints, &server_info)) != 0) {
        std::cerr << "Error: " << gai_strerror(return_val) << "n";
        exit(1);
    }

    // get a socket file descriptor 
    sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (sockfd == -1) {
        std::cerr << "Error " << strerror(errno) << "\n";
        exit(2);
    }

    // connect socket to server
    if (connect(sockfd, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        close(sockfd);
        std::cerr << "Error " << strerror(errno) << "\n";
        exit(3);
    }

    std::cout << "Connected to server" << std::endl;

    socketStream = new google::protobuf::io::FileInputStream(sockfd);
}

Client::~Client() {
    delete socketStream;
    close(sockfd);
}

// message has to be terminated by newline bc we read messages on server using in.readLine()
bool Client::sendData(protoTypes::TestMessage message) {
    using namespace google::protobuf::util;

    if (!SerializeDelimitedToFileDescriptor(message, sockfd)) {
        std::cerr << "Error: SerializeDelimitedToFileDescriptor didn't work\n";
        return false;
    }

    return true;
}

bool Client::receiveData() {
    using namespace google::protobuf::util;

    protoTypes::TestMessage messageFromServer;
    if (!ParseDelimitedFromZeroCopyStream(&messageFromServer, socketStream, NULL)) {
        std::cerr << "Error: ParseDelimitedFromZeroCopyStream didn't work\n";
        return false;
    }

    switch (messageFromServer.command()) {
        case protoTypes::TestMessage::FINISH:
            std::cout << "FROM SERVER: FINISH\n";
            break;
        case protoTypes::TestMessage::EM_STOP:
            std::cout << "FROM SERVER: EM_STOP\n";
            break;
        default:
            std::cout << "UNRECOGNIZED INPUT FROM SERVER\n";
            break;
    }

    return true;
}

}  // namespace client
}  // namespace hyped
