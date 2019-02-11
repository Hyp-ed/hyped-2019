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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "utils/system.hpp"
#include <thread>
#include <iostream>
#include <cerrno>
#include <cstring>

#define PORT 9090
#define SERVER_IP "localhost"
#define BUFFER_SIZE 1024

using hyped::utils::System;

void Read(int sockfd)
{
    char buffer[BUFFER_SIZE];

    while (true) {
        std::memset(&buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, BUFFER_SIZE, 0) <= 0) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            exit(5);
        }

        std::cout << "FROM SERVER: " << buffer;
    }
}


int main(int argc, char *argv[])
{
    System::parseArgs(argc, argv);
    System& sys = System::getSystem();

    int sockfd;
    struct sockaddr_in serv_addr;  // struct containing an internet address (server in this case)
    struct hostent *server;

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    // resolve host address (convert from symbolic name to IP)
    server = gethostbyname(SERVER_IP);
    if (server == NULL) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(2);
    }

    // server address stuff
    std::memset(&serv_addr, 0, sizeof(serv_addr));  // initialize to zeroes
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    std::cout << "Waiting to connect to server..." << std::endl;

    // connect to the server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(3);
    }

    std::cout << "Connected to server" << std::endl;

    // start message reading thread to run in background
    std::thread threadObj(Read, sockfd);

    // send messages
    char *msg = "hello from client\n";
    int len = strlen(msg);
    for (int i = 0; i < 10000000; i++) {
        if (send(sockfd, msg, len, 0) < 0) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            exit(4);
        }
    }

    // wait for message reading thread to finish
    threadObj.join();

    close(sockfd);
    return 0;
}
