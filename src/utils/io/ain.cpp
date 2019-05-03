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

constexpr int kBufSize  = 64;     // TODO(anyone): change to appropriate size

AIN::AIN(uint32_t pin, Logger& log)
    : pin_(pin),
      log_(log)
{}

uint8_t readHelper(int fd)
{
  char buf[2];
  lseek(fd, 0, SEEK_SET);                      // reset file pointer
  read(fd, buf, sizeof(buf));                  // actually consume new data, changes value in buffer
  return std::atoi(buf);
}

// TODO(Greg): incorporate initiasation/setup methods if needed

int AIN::read()
{  
    int fd_;                // file descriptor
    char buf[kBufSize];     // file buffer

    // Create the file path by concatenating the ADC pin number to the end of the string
    // Stores the file path name string into "buf"
    snprintf(buf, sizeof(buf), "/sys/devices/ocp.2/helper.14/AIN%d", pin_);     //Concatenate ADC file name
    
    fd_ = open(buf, O_RDONLY);     // read only
    
    if (fd_ < 0) {
      log_.ERR("AIN", "problem opening ADC");
    }
    
    int val = ain::readHelper(fd_);
    // read(fd, &val, 4);     // read up to 4 digits 0-1799) 
    close(fd_);     // close file and stop reading  
    return val;
}

}   // namespace ain
}}}   // namespace hyped::utils::io