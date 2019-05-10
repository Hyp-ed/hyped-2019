#ifndef UTILS_IO_ADC_HPP_
#define UTILS_IO_ADC_HPP_

#include <cstdint>
#include <vector>

#include "utils/utils.hpp"
#include "logger.hpp"

namespace hyped {
namespace utils {
// Forward declaration
class Logger;
namespace io {

namespace adc {
// constexpr uint8_t kBankNum = 4;
}   // namespace adc

class ADC {
 public:
  ADC(uint32_t pin);
  ADC(uint32_t pin, Logger& log);
  uint8_t read();

private:
  ADC() = delete;

  void initialise();

  void uninitialise();

  void exportBuffer();

  static bool initialised_;

  static std::vector<uint32_t> exported_pins;
  uint32_t pin_;
  Logger& log_;
  int fd_;
};
}}}

#endif  // UTILS_IO_ADC_HPP_