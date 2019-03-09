#include "retractor_manager.hpp"

namespace hyped {
    namespace embrakes {
        Retractor::Retractor(Pins *pins,Logger& log) : log_(log)
        {
            pins_ = new Pins[sizeof(pins)];
            std::cout << "Size of array: " << sizeof(pins_) << std::endl;
            //log.DBG1("Embrakes",)
        }

        //Start two threads, which handle the retraction of the embrakes, join them and wait until both
        //are completed --> then return 0 if there was no error
        //have an atomic int, which the threads write to --> check when both are finished and
        //return the according response. 0 for everything is fine
        int Retractor::retract()
        {
            
        }
    }
}