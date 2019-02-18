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
#include "utils/concurrent/thread.hpp"

namespace hyped {

    namespace motor_control {
        using utils::io::Can;
        using utils::concurrent::BusyThread;
        using utils::Logger;
        using utils::concurrent::Thread;
        using utils::io::CanProccesor;

        //This is used as the datatype of the queue.
        //Type = 0 Sdo Message
        //Type = 1 Pdo Message
        struct Message {
            utils::io::can::Frame& msg;
            int type;
        };

        class CanSender : public Thread
        {
            public:
                CanSender(Logger& log_);
                //CanSender(ControllerInterface* controller);

                void run() override;
                
            protected:
                std::mutex* queueMutex;
                std::condition_variable* queueConditionVar;
                bool processingMessage;
                Logger& log_;
            };

    }
}


#endif //HYPED_2019_CANSENDER_HPP


