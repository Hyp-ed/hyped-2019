/*
* Author: Gregor Konzett
* Organisation: HYPED
* Date: 31.3.2019
* Description: Implements a fake retractor, which simulates the retracting process
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

#include "fake_retractor.hpp"

namespace hyped
{
namespace embrakes
{

FakeRetractor::FakeRetractor(std::atomic<StatusCodes> *status)
{
  status_=status;
}

void FakeRetractor::run()
{
  *status_ = StatusCodes::STARTED;
  sleep(1000);
  *status_ = StatusCodes::FINISHED;
}

}}  // hyped::embrakes

