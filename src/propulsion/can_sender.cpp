//
// Created by Gregor Konzett on 2019-01-28.
//

#include "can_sender.hpp"


namespace hyped {

    namespace motor_control {

        CanSender::CanSender(Logger& log_) : log_(log_){
            log_.INFO("Motor","CanSender initialized");
        }

        void CanSender::sendMessage(utils::io::can::Frame& message) {
            log_.INFO("Motor","send message");
        }

        void CanSender::processNewData(utils::io::can::Frame& message) {

        }

        bool CanSender::hasId(uint32_t id, bool extended) {
            return true;
        }

        void CanSender::registerController() {

        }

    }}

