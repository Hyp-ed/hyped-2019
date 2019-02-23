#ifndef HYPED_2019_CANSENDERATOMIC_HPP
#define HYPED_2019_CANSENDERATOMIC_HPP

#include "utils/io/can.hpp"
#include "utils/logger.hpp"
#include "sender_interface.hpp"
#include <atomic>
#include <iostream>

namespace hyped {
    namespace motor_control {
        using utils::io::Can;
        using utils::Logger;
        using utils::io::CanProccesor;

        class CanSenderAtomic : public CanProccesor, public SenderInterface {
                
            public:    
                CanSenderAtomic(Logger& log_);
                //CanSender(ControllerInterface* controller,Logger& log_);

                void pushSdoMessageToQueue(utils::io::can::Frame& message) override;

                void registerController() override;

                void processNewData(utils::io::can::Frame& message) override;

                bool hasId(uint32_t id, bool extended) override;

                bool getIsSending();

            private:
                Logger log_;
                //Can& can_;
                std::atomic<bool> isSending;
        };
    }
}

#endif //HYPED_2019_CANSENDERATOMIC_HPP