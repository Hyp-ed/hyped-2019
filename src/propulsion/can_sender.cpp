#include "can_sender.hpp"

namespace hyped {

    namespace motor_control {
        CanSender::CanSender(Logger& log_) : log_(log_)
        {

        }


        void CanSender::run() {
            log_.INFO("CONSUMER","START CONSUMING");

            log_.INFO("CONSUMER","END CONSUMING");
        }
    }
}