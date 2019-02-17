//
// Created by Gregor Konzett on 2019-01-28.
//

#ifndef HYPED_2019_CANSENDER_HPP
#define HYPED_2019_CANSENDER_HPP

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <queue>
#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "propulsion/sender_interface.hpp"

namespace hyped {

    namespace motor_control {
        using utils::io::Can;
        using utils::Logger;
        using utils::io::CanProccesor;

        //This is used as the datatype of the queue.
        //Type = 0 Sdo Message
        //Type = 1 Pdo Message
        struct Message {
            utils::io::can::Frame& msg;
            int type;
        };

        class CanSender : public CanProccesor, public SenderInterface
        {
            friend Can;

            public:
                CanSender(Logger& log_);
                //CanSender(ControllerInterface* controller);

                void pushSdoMessageToQueue(utils::io::can::Frame& message) override;

                void processNewData(utils::io::can::Frame& message) override;

                bool hasId(uint32_t id, bool extended) override;

                void registerController() override;

                void sendMessage();
                
            protected:
                std::mutex queueMutex;
                std::condition_variable queueConditionVar;
                bool processingMessage;
                Logger& log_;
                std::queue<utils::io::can::Frame> queue;
            };

    }
}


#endif //HYPED_2019_CANSENDER_HPP
