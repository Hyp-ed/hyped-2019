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

#ifndef EMBRAKES_FAKE_RETRACTOR_HPP_
#define EMBRAKES_FAKE_RETRACTOR_HPP_

#include <cstdint> 
#include <atomic>
#include "utils/concurrent/thread.hpp"
#include "retractor_interface.hpp"

namespace hyped {

using utils::concurrent::Thread;

namespace embrakes {

enum StatusCodes { ERROR, IDLE, STARTED, FINISHED};

class FakeRetractor : public Thread
{
    public:
        FakeRetractor(std::atomic<StatusCodes> *status);
        void run() override;

    private:
        std::atomic<StatusCodes> *status_;
}; 

}}

#endif  // EMBRAKES_FAKE_RETRACTOR_HPP_
