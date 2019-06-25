/*
 * Author: George Karabassis and Iain Macpherson
 * Organisation: HYPED
 * Date: 1.4.2019
 * Description: Interface for controller classes
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

#ifndef BRAKES_CONTROLLER_INTERFACE_HPP_
#define BRAKES_CONTROLLER_INTERFACE_HPP_

#include <stdint.h>

#include "utils/io/can.hpp"

using hyped::utils::io::can::Frame;

namespace hyped
{

namespace embrakes
{
enum ControllerState {
  kRetracted,
  kClamped,
};

struct ControllerMessage {
  uint8_t       message_data[8];
  int           len = 8;
  char   logger_output[250];
};

class ControllerInterface {
 public:
  virtual void registerController() = 0;
  virtual ControllerState getControllerState() = 0;
  virtual void processNewData(utils::io::can::Frame& message) = 0;
};
}  // namespace embrakes
}  // namespace hyped

#endif  // BRAKES_CONTROLLER_INTERFACE_HPP_