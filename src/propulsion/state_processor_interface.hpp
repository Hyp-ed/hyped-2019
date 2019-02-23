/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date: 
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

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
            virtual void initMotors()=0;

            virtual void enterPreOperational()=0;

            virtual void sendTargetVelocity(int32_t target_velocity)=0;

            virtual MotorVelocity requestActualVelocity()=0;

            virtual void quickStopAll()=0;

            virtual void healthCheck()=0;

            virtual bool getFailure()=0;

            virtual void accelerate()=0;

            //Getters
            virtual bool isInitialized()=0;

            //Setters
            virtual void setInitialized(bool initialized)=0;

        protected:
            virtual void registerControllers()=0;

            virtual void configureControllers()=0;

            virtual void prepareMotors()=0;
        };
    }
}

#endif //HYPED_2019_STATEPROCESSORINTERFACE_HPP