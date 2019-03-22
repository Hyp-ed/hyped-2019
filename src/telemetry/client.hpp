#ifndef TELEMETRY_CLIENT_HPP_
#define TELEMETRY_CLIENT_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>

namespace hyped {
namespace client {

    class Client {
        public:
            Client();
            int sendData(std::string message);
            int receiveData(std::string message);
        private:
            int sockfd;
    };

}  // namespace client
}  // namespace hyped

#endif
