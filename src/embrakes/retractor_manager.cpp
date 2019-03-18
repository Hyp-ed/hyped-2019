#include "retractor_manager.hpp"

namespace hyped {
    namespace embrakes {
        RetractorManager::RetractorManager(Pins *pins,Logger& log) : log_(log)
        {
            // Use System to check if the fake brakes should be used
            // System &sys = System::getSystem();
            status = new std::atomic<StatusCodes>[sizeof(pins)/sizeof(pins[0])];  
            retractors_ = new RetractorInterface*[sizeof(pins)/sizeof(pins[0])]; 

            for(uint i = 0;i <= (sizeof(pins)/sizeof(Pins));i++) {
                status[i] = StatusCodes::IDLE;                
                
                // TODO{gregor}: Add check if fake retractors should be loaded instead
                retractors_[i] = new Retractor(pins[i].activate,pins[i].step,&status[i]);
            }
        }

        void RetractorManager::retract()
        {
            for(uint i = 0; i < (sizeof(status)/sizeof(status[0]));i++) {
                retractors_[i]->start();
            }
        }

        int RetractorManager::getStatus()
        {
            StatusCodes statusCode = StatusCodes::FINISHED;

            for(uint i = 0; i < (sizeof(status)/sizeof(status[0]));i++) {
                if(status[i] < statusCode) {
                    statusCode = status[i];
                }
            }

            return statusCode;
        }
    }
}