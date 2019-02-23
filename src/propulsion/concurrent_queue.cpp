#include "concurrent_queue.hpp"

namespace hyped {

    namespace motor_control {
        ConcurrentQueue::ConcurrentQueue()
        {
            std::cout << "Queue initialized" << std::endl;
        }

        void ConcurrentQueue::push(Frame& message) 
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
            
            while(messageQueue.empty())
            {
                queueConditionVar.wait(&lock);
            }

            Frame fr = messageQueue.front();
            messageQueue.pop();

            return fr;
        }

    }
}

