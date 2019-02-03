//
// Created by Gregor Konzett on 2019-01-28.
//

#include "can_sender.hpp"


namespace hyped {

    namespace motor_control {

        CanSender::CanSender(Logger& log_) : log_(log_){
            log_.INFO("Motor","CanSender initialized");
        }

        bool CanSender::pushSdoMessageToStack(utils::io::can::Frame& message) {
            log_.INFO("Motor","SDO Message pushed to stack");
            return true;
        }

        bool CanSender::pushPdoMessageToStack(utils::io::can::Frame& message) {
            log_.INFO("Motor","PDO message pushed to stack");
            return true;
        }

        void CanSender::processNewData(utils::io::can::Frame& message) {

        }

        bool CanSender::hasId(uint32_t id, bool extended) {
            return true;
        }

        void CanSender::registerController() {

        }

    }}

