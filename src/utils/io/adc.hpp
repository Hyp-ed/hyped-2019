/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
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

#ifndef UTILS_IO_ADC_HPP_
#define UTILS_IO_ADC_HPP_

#include <cstdint>
#include <vector>

#include "utils/utils.hpp"
#include "utils/logger.hpp"

namespace hyped {
namespace utils {
// Forward declaration
class Logger;
namespace io {

namespace adc {
}   // namespace adc

class ADC {
 public:
 /**
  * @brief Construct a new ADC object when logger no initialised
  *
  * @param pin
  */
  explicit ADC(uint32_t pin);

  /**
   * @brief Construct a new ADC object with logger for debugging purposes
   *
   * @param pin
   * @param log
   */
  ADC(uint32_t pin, Logger& log);

  /**
   * @brief reads AIN value from file system
   *
   * @return uint16_t return two bytes for [0,4095] range
   */
  uint16_t read();

 private:
  ADC() = delete;

  /**
   * @brief remaps each address for each ADC pin from kADCAddrBase
   * using mmap
   *
   */
  static void initialise();

  /**
   * @brief releases exported ADC pins
   *
   */
  static void uninitialise();

  /**
   * @brief enables ADC pin for analog input through file system
   *
   */
  void exportADC();

  /**
   * @brief configure device register pointers for memory mapping
   *
   */
  void attachADC();

  /**
   * @brief enables ADC data buffer, sets length to 100 for continuous reading
   *
   */
  void setupBuffer();

  /**
   * @brief if specific pin is initialised
   *
   */
  static bool initialised_;
  /**
   * @brief used in initialise() and attachADC() for memory mapping
   *
   */
  static void* base_mapping_;

  /**
   * @brief keeps track of exported pins
   *
   */
  static std::vector<uint32_t> exported_pins;

  uint32_t      pin_;
  Logger&       log_;

  uint32_t      pin_mask_;
  volatile      uint32_t* data_;       // data register
  int           fd_;
};
}}}

#endif  // UTILS_IO_ADC_HPP_
