#include "retractor_manager.hpp"

namespace hyped {
    namespace embrakes {
        RetractorManager::RetractorManager(Pins *pins,Logger& log) : log_(log)
        {
            // Use System to check if the fake brakes should be used
            // System &sys = System::getSystem();


            retractorAmount = sizeof(pins)/sizeof(uint16_t);

            status = new std::atomic<StatusCodes>[retractorAmount];  
            retractors_ = new RetractorInterface*[retractorAmount]; 

            for(uint i = 0;i < retractorAmount;i++) {
                status[i] = StatusCodes::IDLE;                
                log_.INFO("Embrakes","Make retractor %d", retractorAmount);
                // TODO{gregor}: Add check if fake retractors should be loaded instead
                retractors_[i] = new Retractor(pins[i].activate,pins[i].step,&status[i]);
            }
        }

        void RetractorManager::retract()
        {
            log_.INFO("Embrakes"," %d",retractorAmount);
            for(uint i = 0; i < retractorAmount;i++) {
                status[i] = StatusCodes::STARTED;
                retractors_[i]->start();
                log_.INFO("Embrakes","Retracting brake");
            }
        }

        int RetractorManager::getStatus()
        {
            StatusCodes statusCode = StatusCodes::FINISHED;

            for(uint i = 0; i < retractorAmount;i++) {
                if(status[i] < statusCode) {
                    statusCode = status[i];
                }
            }

            return statusCode;
        }
    }
}