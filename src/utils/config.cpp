/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 3. April 2019
 * Description:
 *
 *    Copyright 2018 HYPED
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

#include "utils/config.hpp"

#include <cstdio>

namespace hyped {
namespace utils {

Config::Config(char* config_file)
{
  // load config file, parse it into data structure
  FILE* file = fopen(config_file, "r");

  // allocate line buffer, read and parse file line by line
  char line[250];
  while (fgets(line, sizeof(line), file) != NULL) {
    printf("%s\n", line);
  }
}

}}  // namespace hyped::utils
