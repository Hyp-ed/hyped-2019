//
// Created by Gregor Konzett on 2019-01-28.
//

#ifndef HYPED_2019_CANHANDLER_HPP
#define HYPED_2019_CANHANDLER_HPP

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <queue>
#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "can_sender.hpp"
#include "propulsion/sender_interface.hpp"

namespace hyped {

    namespace motor_control {
        using utils::io::Can;
        using utils::Logger;
        using utils::concurrent::Thread;
        using utils::io::CanProccesor;

        class CanHandler : public CanProccesor, public SenderInterface
        {
            friend Can;

            public:
                CanHandler(Logger& log_);
                //CanSender(ControllerInterface* controller);

                void pushSdoMessageToQueue(utils::io::can::Frame& message) override;

                void registerController() override;

                void processNewData(utils::io::can::Frame& message) override;

                bool hasId(uint32_t id, bool extended) override;
                
            protected:
                std::mutex queueMutex;
                std::condition_variable queueConditionVar;
                bool processingMessage;
                Logger& log_;
                std::queue<utils::io::can::Frame> queue;
                Thread* sender;
            };

    }
}


#endif //HYPED_2019_CANHANDLER_HPP
