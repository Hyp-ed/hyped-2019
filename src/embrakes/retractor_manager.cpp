#include "retractor_manager.hpp"

namespace hyped {
    namespace embrakes {
        RetractorManager::RetractorManager(uint breakAmount, Pins *pins,Logger& log) : log_(log)
        {
            // Use System to check if the fake brakes should be used
            // System &sys = System::getSystem();


            breakAmount_ = breakAmount;

            status = new std::atomic<StatusCodes>[breakAmount_];  
            retractors_ = new RetractorInterface*[breakAmount_]; 

            bool useFakeController = false;

            for(uint i = 0;i < breakAmount_;i++) {
                status[i] = StatusCodes::IDLE;                
                                
                if (useFakeController) {
                    retractors_[i] = new FakeRetractor(&status[i]);
                } else {
                    retractors_[i] = new Retractor(pins[i].activate,pins[i].step,pins[i].push, &status[i]);
                }
            }
        }

        void RetractorManager::retract()
        {
            for(uint i = 0; i < breakAmount_;i++) {
                status[i] = StatusCodes::STARTED;
                retractors_[i]->start();
                log_.INFO("Embrakes","Retracting brake");
            }
        }

        int RetractorManager::getStatus()
        {
            StatusCodes statusCode = StatusCodes::FINISHED;

            for(uint i = 0; i < breakAmount_;i++) {
                if(status[i] < statusCode) {
                    statusCode = status[i];
                }
            }

            return statusCode;
        }
    }
}