//
// Created by Gregor Konzett on 2019-01-28.
//

#ifndef HYPED_2019_CONCURRENTQUEUE_HPP
#define HYPED_2019_CONCURRENTQUEUE_HPP

#include "utils/io/can.hpp"
#include <queue>
#include <iostream>
#include <memory>
#include <mutex>
#include "utils/concurrent/condition_variable.hpp"
#include "utils/concurrent/lock.hpp"

namespace hyped {

    namespace motor_control {
        using utils::io::can::Frame;
        using utils::concurrent::ConditionVariable;
        using utils::concurrent::Lock;

        class ConcurrentQueue
        {
            public:
                ConcurrentQueue();
                void push(Frame& message);
                Frame pop();
                bool getCanRead();

            private:
                Lock lock;
                ConditionVariable queueConditionVar;
                std::queue<Frame> messageQueue;
                bool canRead;
        };
    }
}


#endif //HYPED_2019_CONCURRENTQUEUE_HPP
