/*
 * Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class to read motor configuration data registers from a txt file
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

/*
 * TODO(Iain): reimplement to recieve a path to a file and then iterate through all the
 *             messages to initialise them.
 */
#include <string>
#include <vector>

#include "propulsion/file_reader.hpp"

namespace hyped {
namespace motor_control {
Logger log_(true, 0);

bool FileReader::readFileData(ControllerMessage messages[], const char* filepath)
{
  std::ifstream datafile;
  datafile.open(filepath);

  if (!datafile.is_open()) {
    log_.INFO("FILE_READER", "Unable to open file %s", filepath);
    return false;
  } else {
    std::string line;
    std::string lineData[8];
    int m = 0;

    while (getline(datafile, line)) {
      if (line.empty()) {
      } else if (line.front() == '#') {
      } else {
        if (line.front() == '>') {
            messages[m].logger_output = line.substr(1).c_str();
            log_.INFO("FR", line.substr(1).c_str(), 0);
        } else if (line.front() == '0') {
          splitData(line, lineData);
          addData(lineData, messages[m].message_data);
          m++;
        }
      }
    }
  }
  datafile.close();
  return true;
}

void FileReader::splitData(std::string line, std::string lineData[])
{
  std::vector<std::string> tokens;
  std::stringstream check1(line);
  std::string intermediate;
  while (getline(check1, intermediate, ' ')) {
    tokens.push_back(intermediate);
  }
  for (int i = 0; i < (signed) tokens.size(); i++) {
    lineData[i] = tokens[i];
  }
}

void FileReader::addData(std::string* lineData, uint8_t* message_data)
{
  for (int i = 0; i < 8; i++) {
    message_data[i] = std::stoi(lineData[i], nullptr, 16);
  }
}
}}  // namespace hyped::motor_control
