/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date: 
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

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
