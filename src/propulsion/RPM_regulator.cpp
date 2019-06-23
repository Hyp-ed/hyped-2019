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
  : log_(log)
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
  // testing
  fclose(fp);
  for (int i = 0; i < static_cast<int>(values->size()); i++) {
    log_.INFO("MOTOR", "%d", values->at(i));
  }
}

int32_t RPM_Regulator::calculateRPM(int32_t rpm, int32_t current, int32_t temp)
{
  return 0;
}
}}  // namespace hyped::motor_control
