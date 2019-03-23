#ifndef TELEMETRY_CLIENT_HPP_
#define TELEMETRY_CLIENT_HPP_

#include "types/message.pb.h"
#include <string>

namespace hyped {
namespace client {

    constexpr auto port = "9090";
    constexpr auto server_ip = "localhost";
    // constexpr auto server_ip = "192.168.1.50";

    class Client {
        public:
            Client();
            ~Client();
            bool sendData(protoTypes::TestMessage message);
            bool receiveData();
        private:
            int sockfd;
    };

}  // namespace client
}  // namespace hyped

#endif
