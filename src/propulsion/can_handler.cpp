//
// Created by Gregor Konzett on 2019-01-28.
//

#include "can_handler.hpp"


namespace hyped {

    namespace motor_control {

        CanHandler::CanHandler(Logger& log_) : log_(log_){
            log_.INFO("Motor","CanSender initialized");


            //Start Sending Thread
            sender = new CanSender(log_);
            sender->start();
        }

        void CanHandler::pushSdoMessageToQueue(utils::io::can::Frame& message) {
            //log_.INFO("Motors",message.id+" before queue");

            queue.push(message);

            log_.INFO("Motor","send message");

            utils::io::can::Frame front = queue.front();

            log_.INFO("Motor",front.id+" from queue");
        }

        void CanHandler::processNewData(utils::io::can::Frame& message) {

        }

        bool CanHandler::hasId(uint32_t id, bool extended) {
            return true;
        }

        void CanHandler::registerController() {

        }
    }}

