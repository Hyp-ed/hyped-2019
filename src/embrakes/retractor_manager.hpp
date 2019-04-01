/*
 * Author: Gregor Konzett
 * Organisation: HYPED
 * Date:
 * Description: Main file for navigation class.
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

#ifndef EMBRAKES_RETRACTOR_MANAGER_HPP_
#define EMBRAKES_RETRACTOR_MANAGER_HPP_

#include <cstdint>
#include <iostream>
#include <atomic>
#include "retractor.hpp"
#include "retractor_interface.hpp"
#include "fake_retractor.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped {

using utils::Logger;
using utils::System;

namespace embrakes {

struct Pins {
    uint32_t activate;
    uint32_t step;
    uint32_t push;
};

class RetractorManager
{
    public:
        /*
         * @brief Assigns an array of gpio pins for the stepper motors to the class
        */
        RetractorManager(uint breakAmount, Pins *pins,Logger& log);

        /*
         * @brief Starts the retracting process of the embrakes
        */
        void retract();

        /**
         * @brief Returns if an error occured while retracting the brakes
         * @returns 0 Error, 1 Idle, 2 Started, 3 Finished
        */
        int getStatus();

    private:
        Pins *pins_;
        Logger &log_;
        RetractorInterface **retractors_;
        std::atomic<StatusCodes> *status;
        bool retracting;
        uint breakAmount_;
}; 

}}

#endif  // EMBRAKES_RETRACTOR_MANAGER_HPP_
