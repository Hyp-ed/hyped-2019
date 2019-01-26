//
// Created by Gregor Konzett on 2019-01-26.
//

#ifndef HYPED_2019_STATEPROCESSORINTERFACE_HPP
#define HYPED_2019_STATEPROCESSORINTERFACE_HPP


#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped {

    namespace motor_control {

        using utils::System;
        using utils::Logger;

        //TODO: change to array (ask state machine how they want the data
        struct MotorVelocity {
            int32_t velocity_1;
            int32_t velocity_2;
            int32_t velocity_3;
            int32_t velocity_4;
            int32_t velocity_5;
            int32_t velocity_6;
        };

        class StateProcessorInterface {
        public:
            explicit StateProcessorInterface(Logger& log);

            virtual void initMotors();

            virtual void enterPreOperational();

            virtual void sendTargetVelocity(int32_t target_velocity);

            virtual MotorVelocity requestActualVelocity();

            virtual void quickStopAll();

            virtual void healthCheck();

            virtual bool getFailure();

            //Getters
            bool isInitialized() const {return initialized; };

            //Setters
            void setInitialized(bool initialized) {this->initialized = initialized; };

        private:
            virtual void registerControllers();

            virtual void configureControllers();

            virtual void prepareMotors();


            int motorAmount;
            bool initialized;
        };
    }
}

#endif //HYPED_2019_STATEPROCESSORINTERFACE_HPP