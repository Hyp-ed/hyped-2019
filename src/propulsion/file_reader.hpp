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

#ifndef PROPULSION_FILE_READER_HPP_
#define PROPULSION_FILE_READER_HPP_

#include <fstream>
#include <string>

#include "utils/logger.hpp"

namespace hyped {
namespace motor_control {

using hyped::utils::Logger;

class FileReader {
 public:
  /**
   * @brief Construct a new File Reader that reads data from a file to be sent as a CAN message.
   * @param log
   */
  explicit FileReader(Logger& log);
  /**
   * @brief read data from file and write into the array message.
   *
   * @param message
   * @param len
   */
  void readFileData(uint8_t* message, int32_t len);

 private:
  /**
   * @brief helper function to split the space separated data into individual hex values.
   *
   * @param str
   * @param message
   * @param len
   */
  void splitAndAddData(std::string str, uint8_t* message, int32_t len);
  Logger& log_;
};
}}  // namespace hyped::motor_control
#endif  // PROPULSION_FILE_READER_HPP_
