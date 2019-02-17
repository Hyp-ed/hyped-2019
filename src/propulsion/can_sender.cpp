//
// Created by Gregor Konzett on 2019-01-28.
//

#include "can_sender.hpp"


namespace hyped {

    namespace motor_control {

        CanSender::CanSender(Logger& log_) : log_(log_){
            log_.INFO("Motor","CanSender initialized");
        }

        void CanSender::pushSdoMessageToQueue(utils::io::can::Frame& message) {
            //log_.INFO("Motors",message.id+" before queue");

            queue.push(message);

            log_.INFO("Motor","send message");

            utils::io::can::Frame front = queue.front();

            log_.INFO("Motor",front.id+" from queue");
        }

        void CanSender::processNewData(utils::io::can::Frame& message) {

        }

        bool CanSender::hasId(uint32_t id, bool extended) {
            return true;
        }

        void CanSender::registerController() {

        }

        void CanSender::sendMessage() {
            while(true) {
                log_.INFO("CONSUMER","Waiting");
                std::unique_lock<std::mutex> lck(queueMutex);
                queueConditionVar.wait(lck,processingMessage);
                log_.INFO("CONSUMER","CONSUMING");
                //Sleep here
            }
        }

    }}

