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

#include "utils/io/adc.hpp"


#include <sys/ioctl.h>
//
#include <poll.h>
#include <errno.h>
#include <fcntl.h>      // define O_WONLY and O_RDONLY
#include <unistd.h>     // close()
#include <sys/mman.h>
//
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
//
#include <cstdlib>
#include <cstring>


#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped {
namespace utils {
namespace io {
namespace adc {

constexpr uint32_t kADCAddrBase = 0x44E0D000;
constexpr uint32_t kMmapSize = 0x2000;          // (8 KB)
constexpr int kBufSize  = 100;

uint16_t readHelper(int fd)
{
  char buf[4];                                 // buffer size 4 for fs value
  lseek(fd, 0, SEEK_SET);                      // reset file pointer
  read(fd, buf, sizeof(buf));                  // actually consume new data, changes value in buffer
  return std::atoi(buf);
}

}   // namespace adc

ADC::ADC(uint32_t pin)
    : ADC(pin, System::getLogger())
{ /* EMPTY, delegate to the other constructor */ }

ADC::ADC(uint32_t pin, Logger& log)
    : pin_(pin),
      log_(log),
      initialised_(false),
      fd_(0)
{
  enableADC();
}

void ADC::enableADC()
{
  // int fd;                       // file descriptor
  char buf[adc::kBufSize];      // file buffer
  uint32_t len;

  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/scan_elements/in_voltage%i_en", pin_);          // NOLINT [whitespace/line_length]
  fd_ = open(buf, O_WRONLY);
  if (fd_ < 0) {
    log_.ERR("ADC", "could not enable pin %d", pin_);
  }
  char pinbuf[100];
  snprintf(pinbuf, sizeof(pinbuf), "%i", pin_);
  len = write(fd_, pinbuf, strlen(pinbuf) + 1);
  // close(fd_);
  if (len != strlen(pinbuf) +1) {
    log_.INFO("ADC", "could not enable ADC %d, might be already be enabled", pin_);
    // return;
  } else {
    log_.INFO("ADC", "pin %d was successfully exported", pin_);
    initialised_ = true;
  }
  return;
}

uint16_t ADC::read()
{
  if (!initialised_) {
    log_.ERR("ADC", "ADC not enabled via file system");
    return 0;
  }
  // int fd;
  char buf[100];
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/in_voltage%i_raw", pin_);
  fd_ = open(buf, O_RDONLY);
  if (fd_ < 0) {
    log_.ERR("ADC", "problem reading pin %d raw voltage", pin_);
  }
  log_.DBG1("ADC", "fd: %d", fd_);
  uint16_t val = adc::readHelper(fd_);
  log_.DBG1("ADC", "val: %d", val);
  close(fd_);
  return val;
}


}}}   // namespace hyped::utils::io
