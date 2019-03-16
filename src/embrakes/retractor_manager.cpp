#include "retractor_manager.hpp"

namespace hyped {
    namespace embrakes {
        RetractorManager::RetractorManager(Pins *pins,Logger& log) : log_(log)
        {
            status = new std::atomic<StatusCodes>[sizeof(pins)/sizeof(pins[0])];  
            retractors_ = new Retractor*[sizeof(pins)/sizeof(pins[0])]; 

            for(int i = 0;i <= (sizeof(pins)/sizeof(Pins));i++) {
                status[i] = StatusCodes::IDLE;
                std::cout << i << " " << pins[i].activate << " " << pins[i].step << " " << status[i] << std::endl;
                retractors_[i] = new Retractor(pins[i].activate,pins[i].step,&status[i]);
            }
        }

        //Start two threads, which handle the retraction of the embrakes, join them and wait until both
        //are completed --> then return 0 if there was no error
        //have an atomic int, which the threads write to --> check when both are finished and
        //return the according response. 0 for everything is fine
        void RetractorManager::retract()
        {
            for(int i = 0; i < (sizeof(status)/sizeof(status[0]));i++) {
                retractors_[i]->start();
            }
        }

        int RetractorManager::getStatus()
        {
            StatusCodes statusCode = StatusCodes::FINISHED;

            for(int i = 0; i < (sizeof(status)/sizeof(status[0]));i++) {
                if(status[i] < statusCode) {
                    statusCode = status[i];
                }
            }

            return statusCode;
        }
    }
}