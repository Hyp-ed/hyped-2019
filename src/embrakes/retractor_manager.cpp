#include "retractor_manager.hpp"

namespace hyped {
    namespace embrakes {
        RetractorManager::RetractorManager(Pins pins[EMBRAKEAMOUNT],Logger& log) : log_(log)
        {
            errors = new std::atomic<int>[sizeof(pins)/sizeof(pins[0])];  
            retractors_ = new Retractor*[sizeof(pins)/sizeof(pins[0])]; 

            for(int i = 0;i <= (sizeof(pins)/sizeof(Pins));i++) {
                errors[0] = 0;
                retractors_[i] = new Retractor(pins[i].activate,pins[i].step,&errors[0]);
            }
        }

        //Start two threads, which handle the retraction of the embrakes, join them and wait until both
        //are completed --> then return 0 if there was no error
        //have an atomic int, which the threads write to --> check when both are finished and
        //return the according response. 0 for everything is fine
        int RetractorManager::retract()
        {
            for(int i = 0; i <= (sizeof(errors)/sizeof(errors[0]));i++) {
                
            }

            return 0;
        }
    }
}