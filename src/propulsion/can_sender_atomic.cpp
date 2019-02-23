#include "can_sender_atomic.hpp"

namespace hyped {
    namespace motor_control {
        CanSenderAtomic::CanSenderAtomic(Logger& log_) : log_(log_)//, can_(Can::getInstance())
        {
            isSending=false;
            //can_.start();
        }

        void CanSenderAtomic::pushSdoMessageToQueue(utils::io::can::Frame& message)
        {
            while(isSending);
            std::cout << "sending" << std::endl;   
            //can_.send(message);
            isSending=true;
        }   

        void CanSenderAtomic::registerController()
        {
            //can_.registerProcessor(this); 
        }  

        void CanSenderAtomic::processNewData(utils::io::can::Frame& message)
        {
            std::cout << "processNewData" << std::endl;
            isSending=false;
        }

        bool CanSenderAtomic::hasId(uint32_t id, bool extended)
        {

        }

        bool CanSenderAtomic::getIsSending()
        {
            return isSending;
        }
    }
}