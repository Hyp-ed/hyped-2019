#ifndef TELEMETRY_CLIENT_HPP_
#define TELEMETRY_CLIENT_HPP_

#include <string>

namespace hyped {
namespace client {

    constexpr auto port = "9090";
    constexpr auto server_ip = "localhost";
    // constexpr auto server_ip = "192.168.1.50";

    class Client {
        public:
            Client();
            int sendData(std::string message);  // will need to take a protobuf message in the future
            int receiveData(std::string message);  // will need to take a protobuf message in the future
        private:
            int sockfd;
    };

}  // namespace client
}  // namespace hyped

#endif
