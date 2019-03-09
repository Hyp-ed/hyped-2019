#include "retractor_manager.hpp"

namespace hyped {
    namespace embrakes {
        RetractorManager::RetractorManager(Pins pins[EMBRAKEAMOUNT],Logger& log) : log_(log)
        {
            pins_ = new Pins[sizeof(pins)/sizeof(pins[0])];

            for(int i = 0;i <= (sizeof(pins)/sizeof(Pins));i++) {
                pins_[i] = pins[i];
            }
        }

        //Start two threads, which handle the retraction of the embrakes, join them and wait until both
        //are completed --> then return 0 if there was no error
        //have an atomic int, which the threads write to --> check when both are finished and
        //return the according response. 0 for everything is fine
        int RetractorManager::retract()
        {
            for(int i = 0;i <= (sizeof(pins_)/sizeof(pins_[0]));i++) {
                std::cout << "Elem " << i << " Activate: " << pins_[i].activate << " Step: " << pins_[i].step << std::endl;
            }

            return 0;
        }
    }
}