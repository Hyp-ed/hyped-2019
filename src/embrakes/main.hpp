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

#ifndef EMBRAKES_MAIN_HPP_
#define EMBRAKES_MAIN_HPP_

#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "data/data.hpp"
#include "retractor_manager_interface.hpp"
#include "retractor_manager.hpp"

namespace hyped {

using utils::concurrent::Thread;
using data::Data;
using data::State;
using utils::Logger;
using utils::System;

namespace embrakes {


class Main : public Thread 
{
    public:
        Main(uint8_t id, Logger &log);
	    void run() override;
        bool isRetracted();

    private:
        State currentState;
        RetractorManagerInterface *retractorManager;
        Logger &log_;
};

}}

#endif  // EMBRAKES_MAIN_HPP_
