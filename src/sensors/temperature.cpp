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

#include <stdio.h>
#include "utils/concurrent/thread.hpp"
#include "utils/io/i2c.hpp"
#include "utils/timer.hpp"
#include "sensors/temperature.hpp"

constexpr uint8_t i2c_addr = 0x48;      // for A0, A1 pins low

constexpr uint8_t kConfig = 0x03;      // assume big endian
constexpr uint8_t kReset  = 0x2F;
constexpr uint8_t kStatus = 0x02;
constexpr uint8_t kTempStatus = 0x1;

constexpr uint8_t kTempMSB = 0x00;
constexpr uint8_t kTempLSB = 0x01;

namespace hyped {

using data::Data;
using data::TemperatureData;
using utils::concurrent::Thread;
using utils::io::I2C;
using hyped::utils::Logger;

namespace sensors {

Temperature::Temperature(utils::Logger& log)
     : sys_(utils::System::getSystem()),
       log_(log),
       i2c_(I2C::getInstance())
{
  configure();
}

/**
 * @brief configure for overtemperature and undertemperature interrupts
 *
 */
void Temperature::configure()
{
  // software reset
  writeByte(kReset, 0x2F);
  // set CT and INT pins to active high, set interrupt mode and 16-bit resolution
  // left in continuous mode, DO NOT SET IN ONE-SHOT MODE
  writeByte(kConfig, 0x39);       // set high bits 2,3,4,7
}

void Temperature::writeByte(uint8_t write_reg, uint8_t write_data)
{
  // i2c_.write(write_reg, &write_data, 1);

  uint8_t buffer[2];
  buffer[0] = write_reg;
  buffer[1] = write_data;
  i2c_.write(i2c_addr, buffer, 2);
}

void Temperature::readByte(uint8_t read_reg, uint8_t *read_data)
{
  i2c_.write(i2c_addr, read_data, 1);
  i2c_.read(i2c_addr, &read_reg, 1);
  // i2c_.read(read_reg, read_data, 1);
}

void Temperature::readBytes(uint8_t read_reg, uint8_t *read_data, uint8_t length)
{
  i2c_.write(i2c_addr, read_data, 1);
  i2c_.read(i2c_addr, &read_reg, length);
}

/**
 * @brief get just the two byte temperature data, no interrupts
 *
 */
void Temperature::checkSensor()
{
  // bit 7 to low when temperature converted and sent to register, back to 1 when read
  uint8_t response;
  uint8_t buffer[2];

  uint8_t data_mask = 0x7FFF;
  uint8_t sign_mask = 0x8000;
  int sign_bit = 15;
  int16_t temp, sign;

  readByte(kStatus, &response);

  if (response == kTempStatus) {
    readBytes(kTempMSB, reinterpret_cast<uint8_t*>(buffer), 2);    // from count MSB/LSB registers
    // TODO(Greg): convert to little endian?
    // uint16_t temp = (((uint16_t) (buffer[0]&0x0F)) << 8) + (((uint16_t) buffer[1]));

    // convert from twos complement format, TODO(anyone): check syntax
    uint16_t raw_temp = reinterpret_cast<uint16_t>(buffer);
    sign = (int16_t)((raw_temp & sign_mask) >> sign_bit);

    if (sign) {
      temp = (~raw_temp) & data_mask;
      temp_.temp = (short)(temp * -1);    // [NOLINT] set data type
    } else {
      temp = (raw_temp & data_mask);
      temp_.temp = temp;
    }
    temp_.operational = true;
  } else {
    log_.ERR("TEMPERATURE", "Cannot access data yet, waiting.");
    Thread::sleep(200);
  }
  // wait for 5 measurements and average data;
}

// average multiple readings to account for noise
int Temperature::averageData(int data[kAverageSet])
{
  double mean = 0;
  for (int i=0; i<kAverageSet; i++) {
    mean += data[i];
  }
  mean = mean/kAverageSet;
  double sq_sum = 0;
  for (int i=0; i<kAverageSet; i++) {
    sq_sum += pow((data[i]-mean),2);
  }
  double st_dev = sqrt((sq_sum/kAverageSet));

  int accuracy_factor = 2;      // change if needed

  int final_sum = 0;
  int count = 0;
  for (int i=0; i<kAverageSet; i++) {
    if (abs(data[i] - mean) < (st_dev*accuracy_factor)) {
      final_sum += data[i];
      count++;
    }
  }
  return round(final_sum/count);
}

int Temperature::getTemperature()
{
  return temp_.temp;
}

}}  // namespace hyped::sensors
