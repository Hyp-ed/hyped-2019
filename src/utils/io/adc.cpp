#include "utils/io/adc.hpp"

#include <cstdlib>
#include <stdio.h>
#include <cstring>
#include <unistd.h>     //close()
#include <fcntl.h>     //define O_WONLY and O_RDONLY
#include <string>

namespace hyped {
namespace utils {
namespace io {
namespace adc {

constexpr int kBufSize  = 100;
std::vector<uint32_t> exported_pins;              // NOLINT [build/include_what_you_use]


ADC::ADC(uint32_t pin, Logger& log)
    : pin_(pin),
      log_(log),
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

  exportBuffer();
}

uint8_t readHelper(int fd)
{
  char buf[2];
  lseek(fd, 0, SEEK_SET);                      // reset file pointer
  read(fd, buf, sizeof(buf));                  // actually consume new data, changes value in buffer
  return std::atoi(buf);
}

void ADC::initialise()
{  
  Logger log(false, -1);
  int fd;                // file descriptor
  char buf[kBufSize];     // file buffer
  uint32_t len;
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio\:device0/scan_elements/in_voltage%i_en", pin_);    // device0 or device%i
  fd = open(buf, O_WRONLY);     // write only, need to enable with 1
  if (fd < 0) {
    log.ERR("ADC", "problem isolating adc %d", pin_);
  }

  // fd_ = fd;
  // atexit(uninitialise);
  initialised_ = true;
}

void ADC::uninitialise()
{
  int fd;                 // file descriptor
  char buf[kBufSize];     // file buffer
  uint32_t len;
  Logger log(1, 1);
  log.ERR("ADC", "uninitialising");

  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio\:device0/buffer/enable");
  fd = open(buf, O_WRONLY);     // write only, need to disable with 0
  if (fd < 0) {
    log.ERR("ADC", "problem disabling buffer");
  }
  len = write(fd,"0", 1);
  close(fd);
}

void ADC::exportBuffer()
{
  Logger log(false, -1);
  int fd;                 // file descriptor
  char buf[kBufSize];     // file buffer
  uint32_t len;

  // set buffer length
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio\:device0/buffer/length");
  fd = open(buf, O_WRONLY);     // write only
  if (fd < 0) {
    log.ERR("ADC", "problem setting buffer length");
  }
  len = write(fd,"100", 100);     // number of bytes
  close(fd);
  if (len != 100) {
    log_.INFO("ADC", "could not set buffer for ADC %d, might be already exported", pin_);
    // return;
  }

  // enable buffer
  snprintf(buf, sizeof(buf), "/sys/bus/iio/devices/iio\:device0/buffer/enable");
  fd = open(buf, O_WRONLY);     // write only, need to enable with 1
  if (fd < 0) {
    log.ERR("ADC", "problem enabling buffer");
  }
  len = write(fd,"1", 1);     // number of bytes
  close(fd);
  if (len != 1) {
    log_.INFO("ADC", "could not enable buffer for ADC %d", pin_);
    // return;
  }

}

uint8_t ADC::read()
{
  if (!initialised_) {
    log_.ERR("ADC", "service has not been initialised");
    return 0;
  }

  Logger log(false, -1);
  int fd;                 // file descriptor
  char buf[kBufSize];     // file buffer for data
  snprintf(buf,sizeof(buf), "/sys/bus/iio/devices/iio:device0");   // reads buffer
  fd = open(buf,O_RDONLY);

  int val = adc::readHelper(fd_);
  return val;
}

}   // namespace adc
}}}   // namespace hyped::utils::io