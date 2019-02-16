//
// Created by Gregor Konzett on 2019-01-28.
//

#ifndef HYPED_2019_SENDERINTERFACE_HPP
#define HYPED_2019_SENDERINTERFACE_HPP

#include "utils/io/can.hpp"

namespace hyped {

    namespace motor_control {

        class SenderInterface
        {
            public:
                virtual void sendMessage(utils::io::can::Frame& message)=0;
                virtual void registerController()=0;  
        };
    

    }
}


#endif //HYPED_2019_SENDERINTERFACE_HPP
