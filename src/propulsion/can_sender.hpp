//
// Created by Gregor Konzett on 2019-01-28.
//

#ifndef HYPED_2019_CANSENDER_HPP
#define HYPED_2019_CANSENDER_HPP

#include <cstdint>
#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "propulsion/sender_interface.hpp"
#include <queue>

namespace hyped {

    namespace motor_control {
        using utils::io::Can;
        using utils::Logger;
        using utils::io::CanProccesor;

        class CanSender : public CanProccesor, public SenderInterface
        {
            friend Can;

            public:
                CanSender(Logger& log_);
                //CanSender(ControllerInterface* controller);

                bool pushSdoMessageToStack(utils::io::can::Frame& message) override;
            
                bool pushPdoMessageToStack(utils::io::can::Frame& message) override;

                void processNewData(utils::io::can::Frame& message) override;

                bool hasId(uint32_t id, bool extended) override;

                void registerController() override;
                
            protected:
                bool processingSdoMessage;
                bool processingPdoMessage;
                Logger& log_;

                std::queue<utils::io::can::Frame> sdoQueue;
                std::queue<utils::io::can::Frame> pdoQueue;
            };

    }
}


#endif //HYPED_2019_CANSENDER_HPP
