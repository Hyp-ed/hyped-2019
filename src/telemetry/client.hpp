#ifndef TELEMETRY_CLIENT_HPP_
#define TELEMETRY_CLIENT_HPP_

#include <string>

namespace hyped {
namespace client {

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
