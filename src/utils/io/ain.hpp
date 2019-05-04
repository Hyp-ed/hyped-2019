#ifndef UTILS_IO_AIN_HPP_
#define UTILS_IO_AIN_HPP_

#include <cstdint>
#include <vector>

#include "utils/utils.hpp"
#include "logger.hpp"

namespace hyped {
namespace utils {
// Forward declaration
class Logger;
namespace io {

namespace ain {
// constexpr uint8_t kBankNum = 4;
}   // namespace ain

class AIN {
 public:
  AIN(uint32_t pin);
  AIN(uint32_t pin, Logger& log);
  uint8_t read();

private:
  AIN() = delete;

  void initialise();

  void uninitialise();

  static bool initialised_;

  static std::vector<uint32_t> exported_pins;
  uint32_t pin_;
  Logger& log_;
  int fd_;
};
}}}

#endif  // UTILS_IO_AIN_HPP_