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

#include <vector>
#include "propulsion/RPM_regulator.hpp"

namespace hyped {

namespace motor_control {

RPM_Regulator::RPM_Regulator(Logger& log)
  : log_(log),
    current_index(0)
{
  readFile(&optimal_current, CURRENT_FP);
}

void RPM_Regulator::readFile(vector<int32_t>* values, const char* filepath)
{
  FILE* fp;
  fp = fopen(filepath, "r");

  if (fp == NULL) {
    log_.ERR("MOTOR", "Unable to open: %s", filepath);
    exit(EXIT_FAILURE);
  } else {
    char line[255];
    while (fgets(line, static_cast<int>(sizeof(line)/sizeof(line[0])), fp)) {
      values->push_back(static_cast<int32_t>(std::atoi(line)));
    }
  }
}

int32_t RPM_Regulator::calculateRPM(int32_t act_velocity, int32_t act_rpm,
                                    int32_t act_current, int32_t act_temp)
{
  int32_t opt_current = optimal_current.at(current_index);
  int32_t opt_rpm = calculateOptimalRPM(act_velocity);
  if ( (act_current >= (opt_current - kmargin)) && (act_current <= (opt_current + kmargin)) ) {
        current_index++;  // todo(Iain): find a better way to do this
  }
  if (act_current < opt_current &&
      act_temp < MAX_TEMP &&
      act_rpm < opt_rpm) {
    return act_rpm + step(opt_rpm, true);
  } else if (act_current > opt_current ||
             act_temp > MAX_TEMP ||
             act_rpm > opt_rpm) {
    return act_rpm - step(opt_rpm, false);
  } else {
    return act_rpm;
  }
}

int32_t RPM_Regulator::calculateOptimalRPM(int32_t act_velocity)
{
  int32_t opt_rpm = std::round(0.32047 * act_velocity*act_velocity +
                                297.72578 * act_velocity + 1024.30824);
  return opt_rpm;
}

int32_t RPM_Regulator::step(int32_t opt_rpm, bool direction)
{
  if (direction) {
    return std::round(opt_rpm*0.1);  // placeholder
  } else {
    return std::round(opt_rpm*0.1);  // placeholder
  }
}
}}  // namespace hyped::motor_control
