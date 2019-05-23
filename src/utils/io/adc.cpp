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

bool ADC::initialised_ = false;
void* ADC::base_mapping_;
std::vector<uint32_t> ADC::exported_pins;              // NOLINT [build/include_what_you_use]

ADC::ADC(uint32_t pin)
    : ADC(pin, System::getLogger())
{ /* EMPTY, delegate to the other constructor */ }

ADC::ADC(uint32_t pin, Logger& log)
    : pin_(pin),
      log_(log),
      data_(0),
      fd_(0)
{
  if (!initialised_)  initialise();
  // check pin is not already allocated
  for (uint32_t pin : exported_pins) {
    if (pin_ == pin) {
      log_.ERR("ADC", "pin %d already in use", pin_);
      return;
    }
  }
  exported_pins.push_back(pin_);
  exportADC();
  // attachADC();
}

void ADC::initialise()
{
  Logger log(false, -1);
  int fd;                // file descriptor
  void* base;
  // char buf[adc::kBufSize];     // file buffer

  fd = open("/dev/mem", O_RDWR);
  if (fd < 0) {
    log.ERR("ADC", "could not open /dev/mem");
    return;
  }

  base = mmap(0, adc::kMmapSize, PROT_READ | PROT_WRITE, MAP_SHARED,
              fd, adc::kADCAddrBase);
  if (base == MAP_FAILED) {
    log.ERR("ADC", "could not map bank 0x%x", adc::kADCAddrBase);
    return;
  }
  base_mapping_ = base;
  atexit(uninitialise);
  initialised_ = true;
}

void ADC::uninitialise()
{
  int fd;                       // file descriptor
  char buf[adc::kBufSize];      // file buffer
  Logger log(1, 1);
  log.ERR("ADC", "uninitialising");

  for (uint32_t pin : exported_pins) {
    snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/scan_elements/in_voltage%i_en", pin);         // NOLINT [whitespace/line_length]
    fd = open(buf, O_WRONLY);
    if (fd < 0) {
    log.ERR("ADC", "could not disable pin %d", pin);
    }
    char buf[10];
    snprintf(buf, sizeof(buf), "%i", pin);
    write(fd, buf, strlen(buf) + 1);
  }
  close(fd);
}

void ADC::exportADC()
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
  }

  return;
}

void ADC::attachADC()   // TODO(anyone): fix syntax for data_ = base + data
{
  // // uint8_t bank;
  // uint8_t pin_id;

  // // bank      = pin_/32;
  // pin_id    = pin_%32;
  // // corresponds to desired data of pin by indicating specific bit within byte of pin data
  // pin_mask_ = 1 << pin_id;
  // log_.DBG1("ADC", "adc %d resolved as pin, %d", pin_, pin_id);

  // uint32_t base = reinterpret_cast<uint32_t>(base_mapping_);
  // data_  = reinterpret_cast<volatile uint32_t*>(base + kData); // TODO(Greg): implement
  // setupBuffer();
}

void ADC::setupBuffer()
{
  // int fd;                      // file descriptor
  char buf[adc::kBufSize];     // file buffer
  uint32_t len;

  // set buffer length to 100
  fd_ = open("/sys/bus/iio/devices/iio:device0/buffer/length", O_WRONLY);     // write only
  if (fd_ < 0) {
    log_.ERR("ADC", "problem opening length");
  }
  uint32_t out = 100;
  len = write(fd_, &out, sizeof(out));     // TODO(Greg): check syntax
  // close(fd_);
  if (len != 100) {
    log_.INFO("ADC", "could not set buffer length to %d", out);
    // return;
  }

  // enable buffer with 1
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/buffer/enable");
  fd_ = open(buf, O_WRONLY);
  if (fd_ < 0) {
    log_.ERR("ADC", "problem opening enable");
  }
  out = 1;
  len = write(fd_, &out, sizeof(out));     // TODO(Greg): check syntax
  close(fd_);
  if (len != 1) {
    log_.INFO("ADC", "could not enable buffer", pin_);
    // return;
  }
}

uint16_t ADC::read()
{
  if (!initialised_) {
    log_.ERR("ADC", "service has not been initialised");
    return 0;
  }
  // if (!data_) {
  //   log_.ERR("ADC", "data register not configured, pin %d", pin_);
  //   return 0;
  // }

  // int fd;
  char buf[100];
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio:device0/in_voltage%i_raw", pin_);
  fd_ = open(buf, O_RDONLY);
  if (fd_ < 0) {
    log_.ERR("ADC", "problem reading pin %d raw voltage ", pin_);
  }
  log_.DBG1("ADC", "fd: %d", fd_);
  uint16_t val = adc::readHelper(fd_);
  log_.DBG1("ADC", "val: %d", val);
  close(fd_);
  return val;
}


}}}   // namespace hyped::utils::io
