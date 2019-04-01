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

#include "propulsion/file_reader.hpp"

namespace hyped {
namespace motor_control {
  Logger log_(true, 0);

  bool FileReader::readFileData(std::string* messages, const char* filepath)
{
  std::ifstream datafile;
  datafile.open(filepath);

  if (!datafile.is_open()) {
    log_.INFO("FILE_READER", "Unable to open file");
    return false;
  } else {
    std::string line;
    std::string lineData[9];
    int m =0;
    while (getline (datafile, line)) {
      if (line.empty() || line.front() == '#');
      else {
        splitData(line, lineData);
        m++;
      }
    }
    // int m = 0;
    // std::string line;
    // while(getline(datafile, line)) {
    //   if (line.front() == '#' || line.empty());
    //   else {
    //     std::string strData[messages[0].len+1];
    //     splitData(line, messages[0].len+1, strData);
    //     addData(strData, messages[m]);
    //     m++;
    //   }
    // }
  }
  datafile.close();
  return true;
}

void FileReader::splitData(std::string line, std::string* lineData)
{
  // std::string remains = line;
  // int prevIndex = 0;
  // for (int i = 0; i < 8; i ++) {
  //   int index = remains.find(' ');
  //   strData[i] = remains.substr(prevIndex, index);
  //   prevIndex = index+1;
  //   remains = remains.substr(prevIndex);
  // }
  // strData[len] = remains;
}

void FileReader::addData(std::string* lineData, ControllerMessage message)
{
  // for (int i = 0; i < 8; i++) {
  //   message.message_data[i] = (uint8_t) std::stoi(strData[i], nullptr, 16);
  // }
  // message.logger_output = strData[message.len].c_str();
}
}}  // namespace hyped::motor_control
