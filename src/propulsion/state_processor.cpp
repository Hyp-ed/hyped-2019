//
// Created by Gregor Konzett on 2019-01-26.
//

#include "state_processor.hpp"

namespace hyped {

    namespace motor_control {

        StateProcessor::StateProcessor(int motorAmount, Logger &log)
        : log_(log),
          initialized(false),
          sys_(System::getSystem()),
          motorAmount(motorAmount)
        {

            log_.DBG1("Motor","StateProcessor constructor was called");
            useTestControllers = sys_.fake_motors;

            //controllers = new ControllerInterface[motorAmount];

            if(useTestControllers) //Use the test controller implementation
            {

            }
            else //Use real controllers
            {
                for(int i = 0;i<motorAmount;i++) {
                    //controllers[i] = new Controller();
                }
            }
        }

        void StateProcessor::initMotors()
        {
            //Register controllers on CAN bus
            registerControllers();

            //Configure controller parameters
            configureControllers();

            //TODO: Handle errors


            //If no error
            prepareMotors();

            initialized = true;
        }

        void StateProcessor::registerControllers()
        {

        }

        void StateProcessor::configureControllers()
        {

        }

        void StateProcessor::prepareMotors()
        {

        }

        void StateProcessor::enterPreOperational()
        {

        }

        void StateProcessor::accelerate()
        {

        }

        void StateProcessor::sendTargetVelocity(int32_t target_velocity)
        {

        }

        MotorVelocity StateProcessor::requestActualVelocity()
        {
            return MotorVelocity();
        }

        void StateProcessor::quickStopAll()
        {

        }

        void StateProcessor::healthCheck()
        {

        }

        bool StateProcessor::getFailure()
        {
            return false;
        }

        bool StateProcessor::isInitialized()
        {
            return this->initialized;
        }

        void StateProcessor::setInitialized(bool initialized)
        {
            this->initialized = initialized;
        }


    }
}