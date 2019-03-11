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

#include <iostream>  // NO LINT
#include <cerrno>  // NO LINT
#include <cstring>  // NO LINT
#include <sys/socket.h>  // NO LINT
#include <netinet/in.h>  // NO LINT
#include <netdb.h>  // NO LINT
#include <unistd.h>  // NO LINT
#include <thread>  // NO LINT
#include <google/protobuf/io/coded_stream.h>  // NO LINT
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>  // NO LINT
#include <google/protobuf/message_lite.h>  // NO LINT
#include "types/message.pb.h"  // NO LINT

#define PORT 9090
#define SERVER_IP "192.168.1.150" // macbook as client and linux machine as base-station

// reads header containing size encoded as varint, returns this size
google::protobuf::uint32 readHeader(char *buffer)
{
    using namespace google::protobuf::io;

    google::protobuf::uint32 size;  // this will hold the size of our message body in bytes
    ArrayInputStream raw_input(buffer, 4);  // create raw stream containing buffer of varint
    CodedInputStream coded_input_ptr(&raw_input);  // create CodedInput wrapper

    coded_input_ptr.ReadVarint32(&size);  // read size as varint

    return size;
}

// read body of a message, and print out the contents of this body
void readBody(int sockfd, google::protobuf::uint32 body_size)
{
    using namespace google::protobuf::io;

    int bytes_received;
    int header_size = CodedOutputStream::VarintSize32(body_size);  // literal amount of bytes that header takes up
    char buffer[body_size + header_size];
    std::cout << "header_size: " << header_size << std::endl;  // if we don't print this we run into a lot of corrupt messages, really strange; must fix

    // read whole message (header + body) into buffer
    if ((bytes_received = recv(sockfd, buffer, header_size + body_size, 0)) == -1) {
        std::cerr << "Error receiving data (reading body)" << std::endl;
    }
    // if for some reason the bytes we read in don't add up to header_size + bodysize
    // for some reason the bytes we received got messed up somehow and can't be deserialized
    if (header_size + body_size != bytes_received) {
        std::cerr << "Error receiving data (corrupt message)" << std::endl;
        return;  // skip this message
    }

    ArrayInputStream raw_input(buffer, body_size + header_size);  // raw input stream
    CodedInputStream coded_input(&raw_input);  // CodedInput wrapper

    // we have to read body size of message again bc buffer contains header + body (move file position indicator to begin of body)
    // shouldn't change value of uint32 body_size variable we were passed in
    coded_input.ReadVarint32(&body_size);

    CodedInputStream::Limit msg_limit = coded_input.PushLimit(body_size);  // add limit to prevent reading beyond message length
    protoTypes::TestMessage msg;
    msg.ParseFromCodedStream(&coded_input);  // deserialize
    coded_input.PopLimit(msg_limit);  // remove limit

    switch (msg.command()) {
        case protoTypes::TestMessage::FINISH:
            std::cout << "FROM SERVER: " << "FINISH" << std::endl;
            break;
        case protoTypes::TestMessage::EM_STOP:
            std::cout << "FROM SERVER: " << "EM_STOP" << std::endl;
            break;
        default:
            std::cout << "UNRECOGNIZED INPUT FROM SERVER" << std::endl;
    }
}

void Read(int sockfd)
{
    char buffer[4];  // 4 since our varint is highly unlikely to be over 4 bytes (size of message can be up to 2^28 bits)
    int bytes_received = 0;  // we use this so we can see if we are sent nothing/empty or an error occurs

    std::memset(buffer, 0, sizeof(buffer));

    while (true) {
        // below we read first four bytes of message into buffer, buffer should contain varint of body size
        // we only peek into our socket queue bc if the size of our body is less than 2^21 bits
        // this means our varint takes up less than 4 bytes, which means the remaining bytes are actually
        // the beginning of the message body, which we need to process later
        if ((bytes_received = recv(sockfd, buffer, 4, MSG_PEEK)) == -1) {  // error
            std::cerr << "Error receiving data (reading header)" << std::endl;
        }
        else if (bytes_received == 0) {  // empty/nothing/probs connection closed
            std::cerr << "Didn't receive anything, connection probably closed" << std::endl;
            break;
        }

        readBody(sockfd, readHeader(buffer));
    }
}

void send(int sockfd, protoTypes::TestMessage::Command cmd, int data)
{
    using namespace google::protobuf::io;

    // build message
    protoTypes::TestMessage msg;
    msg.set_command(cmd);
    msg.set_data(data);

    int body_size = msg.ByteSizeLong();
    int header_size = CodedOutputStream::VarintSize32(body_size);  // literal amount of bytes that header takes up
    char buffer[header_size + body_size];
    std::memset(buffer, 0, sizeof(buffer));  // just in case; overwrite garbage data

    // create streams that write to our buffer
    ArrayOutputStream raw_output(buffer, header_size + body_size);
    CodedOutputStream *coded_output = new CodedOutputStream(&raw_output);

    // write message size and actual message to buffer
    coded_output->WriteVarint32(body_size);
    msg.SerializeToCodedStream(coded_output);

    send(sockfd, buffer, header_size + body_size, 0);
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr;  //struct containing an internet address (server in this case)
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

    for (int i = 0; i < 1000000; i++) {
        send(sockfd, protoTypes::TestMessage::VELOCITY, 222);
        send(sockfd, protoTypes::TestMessage::ACCELERATION, 333);
        send(sockfd, protoTypes::TestMessage::BRAKE_TEMP, 777);

        send(sockfd, protoTypes::TestMessage::VELOCITY, 333);
        send(sockfd, protoTypes::TestMessage::ACCELERATION, 444);
        send(sockfd, protoTypes::TestMessage::BRAKE_TEMP, 888);

        send(sockfd, protoTypes::TestMessage::VELOCITY, 444);
        send(sockfd, protoTypes::TestMessage::ACCELERATION, 555);
        send(sockfd, protoTypes::TestMessage::BRAKE_TEMP, 999);
    }

    // wait for message reading thread to finish
    threadObj.join();

    close(sockfd);
    return 0;
}
