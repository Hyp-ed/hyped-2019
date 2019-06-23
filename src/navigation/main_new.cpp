/*
 * Author: Lukas Schaefer, Neil McBlane
 * Organisation: HYPED
 * Date: 05/04/2019
 * Description: Main file for navigation class.
 *
 *  Copyright 2019 HYPED
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *  except in compliance with the License. You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under
 *  the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "navigation/main_new.hpp"

namespace hyped {
namespace navigation {

  Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      log_(log),
      sys_(System::getSystem()),
      nav_(log, true) {}  // TODO(Neil) - change to system argument

  void Main::run()
  {
    nav_.determineCalibration();
    while (sys_.running_) {
      nav_.navigate();
    }
  }
}}  // namespace hyped::navigation
