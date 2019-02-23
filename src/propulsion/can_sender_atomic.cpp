#include "can_sender_atomic.hpp"

namespace hyped {
    namespace motor_control {
        CanSenderAtomic::CanSenderAtomic(Logger& log_) : log_(log_)//, can_(Can::getInstance())
        {
            canSend=true;
            //can_.start();
        }

        void CanSenderAtomic::pushSdoMessageToQueue(utils::io::can::Frame& message)
        {
            while(canSend==false);
            
            //can_.send(message);
            canSend=false;
        }   

        void CanSenderAtomic::registerController()
        {
            //can_.registerProcessor(this); 
        }  

        void CanSenderAtomic::processNewData(utils::io::can::Frame& message)
        {
            canSend=true;
        }

        bool CanSenderAtomic::hasId(uint32_t id, bool extended)
        {

        }
    }
}