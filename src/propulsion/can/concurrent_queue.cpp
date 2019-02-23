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

#include "concurrent_queue.hpp"

namespace hyped
{

namespace motor_control
{
ConcurrentQueue::ConcurrentQueue()
{
    std::cout << "Queue initialized" << std::endl;
}

void ConcurrentQueue::push(Frame &message)
{
    std::cout << "Queue push with id:" << message.id << std::endl;
    lock.lock();

    messageQueue.push(message);

    //lock.unlock();

    queueConditionVar.notify();
    lock.unlock();
}

Frame ConcurrentQueue::pop()
{
    std::cout << "Getting first entry" << std::endl;
    lock.lock();

    while (messageQueue.empty())
    {
        queueConditionVar.wait(&lock);
    }

    Frame fr = messageQueue.front();
    messageQueue.pop();

    return fr;
}

} // namespace motor_control
} // namespace hyped
