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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>

constexpr auto port = "9090";
constexpr auto server_ip = "localhost";
// constexpr auto server_ip = "192.168.1.50";

void myRead(int sockfd) {
    char buffer[1024];
    int bytes_received;

    while (true) {
        memset(&buffer, 0, sizeof(buffer));
        if ((bytes_received = recv(sockfd, buffer, 1024, 0)) < 0) {
            std::cerr << "Error " << strerror(errno) << "\n";
            exit(5);
        }

        buffer[bytes_received] = '\0';
        std::cout << "FROM SERVER: " << buffer;
    }
}

int main(void) {
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
    int sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
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

    // read messages
    std::thread readThread {myRead, sockfd};

    // send messages
    char *msg = "hello from client\n";
    int len = strlen(msg);
    for (int i = 0; i < 10000000; i++) {
        if (send(sockfd, msg, len, 0) < 0) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            exit(4);
        }
    }

    readThread.join();
    close(sockfd);
    return 0;
}
