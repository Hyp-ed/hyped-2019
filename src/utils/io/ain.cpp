#include "utils/io/ain.hpp"

#include <cstdlib>
#include <stdio.h>
#include <cstring>
#include <unistd.h>     //close()
#include <fcntl.h>     //define O_WONLY and O_RDONLY

namespace hyped {
namespace utils {
namespace io {
namespace ain {

constexpr int kBufSize  = 100;
std::vector<uint32_t> exported_pins;              // NOLINT [build/include_what_you_use]


AIN::AIN(uint32_t pin, Logger& log)
    : pin_(pin),
      log_(log),
      fd_(0)
{
  if (!initialised_)  initialise();
  // check pin is not already allocated
  for (uint32_t pin : exported_pins) {
    if (pin_ == pin) {
      log_.ERR("GPIO", "pin %d already in use", pin_);
      return;
    }
  }
  exported_pins.push_back(pin_);
}

uint8_t readHelper(int fd)
{
  char buf[2];
  lseek(fd, 0, SEEK_SET);                      // reset file pointer
  read(fd, buf, sizeof(buf));                  // actually consume new data, changes value in buffer
  return std::atoi(buf);
}

void AIN::initialise()
{  
  Logger log(false, -1);
  int fd;                // file descriptor
  char buf[kBufSize];     // file buffer

  // Create the file path by concatenating the ADC pin number to the end of the string
  // Stores the file path name string into "buf"
  snprintf(buf, sizeof(buf), "/sys/devices/ocp.2/helper.14/AIN%d", pin_);     //Concatenate ADC file name
  
  fd = open(buf, O_RDONLY);     // read only
  
  if (fd < 0) {
    log.ERR("AIN", "problem opening ADC");
  }  
  fd_ = fd;
  // atexit(uninitialise);
  initialised_ = true;
}

void AIN::uninitialise()
{
  Logger log(1, 1);
  log.ERR("GPIO", "uninitialising");
  close(fd_);
}

uint8_t AIN::read()
{
  if (!initialised_) {
    log_.ERR("AIN", "service has not been initialised");
    return 0;
  }
  int val = ain::readHelper(fd_);
  return val;
}

}   // namespace ain
}}}   // namespace hyped::utils::io