/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 23/05/18
 * Description: Main file for Imu
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

#ifndef SENSORS_IMU_HPP_
#define SENSORS_IMU_HPP_

#include "sensors/interface.hpp"
#include "utils/logger.hpp"
#include "utils/io/spi.hpp"
#include "utils/io/gpio.hpp"


namespace hyped {

using hyped::utils::io::SPI;
using utils::Logger;
using utils::io::GPIO;
using data::NavigationVector;

namespace sensors {

class Imu : public ImuInterface {
 public:
  Imu(Logger& log, uint32_t pin, uint8_t acc_scale = 0x08, uint8_t gyro_scale = 0x00);
  ~Imu();
  /*
   *  @brief Returns if the sensor is online
   *
   *  @return true if the sensor is online
   */
  bool isOnline() override {
    return whoAmI();
  }
  /*
   *  @brief Get the Imu data and update the pointer
   */
  void getData(ImuData* data) override;

 private:
  /*
   *  @brief Sets the range for the gyroscope
   */
  void setGyroScale(int scale);
  /*
   *  @brief Sets the range for the accelerometer
   */
  void setAcclScale(int scale);
  static const uint64_t time_start;
  void init();
  void select();
  void deSelect();
  bool whoAmI();
  void writeByte(uint8_t write_reg, uint8_t write_data);
  void readByte(uint8_t read_reg, uint8_t *read_data);
  void readBytes(uint8_t read_reg, uint8_t *read_buff, uint8_t length);

 private:
  SPI&    spi_;
  Logger& log_;
  GPIO    gpio_;
  uint8_t acc_scale_;
  uint8_t gyro_scale_;
  double  acc_divider_;
  double  gyro_divider_;
  bool    is_online_;
};

}}  // namespace hyped::sensors


#endif  // SENSORS_IMU_HPP_
