/*
* Author: Kornelija Sukyte
* Organisation: HYPED
* Date:
* Description:
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

#include "embrakes/stepper.hpp"

namespace hyped {
namespace embrakes {

Stepper::Stepper(Logger& log, uint8_t id)
    : log_(log),
      data_(data::Data::getInstance()),
      node_id_(id),
      can_(Can::getInstance())
{
can_.start();    
}

void Stepper::registerStepper()
{
  can_.registerProcessor(this);
}

void ProcessNewData(Frame &message)
{

}

}}  // namespace hyped::embrakes