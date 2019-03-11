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

#include "retractor.hpp"

namespace hyped
{
namespace embrakes
{
    Retractor::Retractor(uint32_t activate,uint32_t step,std::atomic<int> *error)
        : activate_(activate),
        step_(step),
        error_(error)
    {

    }

    void Retractor::run()
    {
        *error_=2;
    }
}
}

