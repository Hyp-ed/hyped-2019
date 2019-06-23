/*
 * Author: George Karabassis
 * Co-Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for the Motor Controller
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef PROPULSION_RPM_REGULATOR_HPP_
#define PROPULSION_RPM_REGULATOR_HPP_

#include <cstdlib>
#include <vector>
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "propulsion/controller.hpp"

namespace hyped {

using std::vector;

using utils::Logger;
using motor_control::Controller;

namespace motor_control {

class RPM_Regulator {
 public:
  /*
  * @brief Construct a new rpm regulator object
  * @param log
  */
  explicit RPM_Regulator(Logger& log);
  /**
   * @brief Calculate the optimal rpm based on criteria from all the motors
   *        as well optimal values produced by simulations.
   *
   * @param rpm - average rpm of all the motors
   * @param current - average current drawn by all the motors
   * @param temp - average temperature of all motors
   * @return int32_t - the optimal rpm which the motors should be set to.
   */
  int32_t calculateRPM(int32_t rpm, int32_t current, int32_t temp);

 private:
  /**
   * @brief reads the optimal values from a file
   *
   * @param filepath - path to a file containing values
   */
  void readFile(vector<int32_t>* values, const char* filepath);

  Logger& log_;
  const char* CURRENT_FP = "data/in/optimal_current.txt";
  vector<int32_t> optimal_current;
};

}}  // namespace hyped::motor_control
#endif  // PROPULSION_RPM_REGULATOR_HPP_
