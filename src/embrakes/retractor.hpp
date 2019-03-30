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

#ifndef EMBRAKES_RETRACTOR_HPP_
#define EMBRAKES_RETRACTOR_HPP_

#include <cstdint> 
#include <atomic>
#include <ctime>
#include "retractor_interface.hpp"
#include "./utils/io/gpio.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::io::GPIO;
using utils::io::gpio::Direction;

namespace embrakes {

#define STEPS 400

class Retractor : public RetractorInterface
{
    public:
        /*
         * @brief {Assigns the stepper motor gpio pins to the class and enables 
         *          it to change the status variable of the base class}
        */
        Retractor(uint32_t activate,uint32_t step, uint32_t pushButton, std::atomic<StatusCodes> *status);

        /*
         * @brief Runs the retracting process
         * @description Updates the status variable when it is finished or when an error occured
        */
        void run() override;

    private:
        float period;
        float rpm;
        uint32_t step_;
        time_t startTime;
        uint32_t activate_;
        uint32_t pushButton_;
}; 

}}

#endif  // EMBRAKES_RETRACTOR_HPP_
