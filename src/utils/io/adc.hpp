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
}   // namespace adc

class ADC {
 public:
  ADC(uint32_t pin);
  ADC(uint32_t pin, Logger& log);
  uint8_t read();

private:
  ADC() = delete;

  static void initialise();

  static void uninitialise();

  void exportADC();
  
  void attachADC();

  void setupBuffer();

  static bool initialised_;

  static void* base_mapping_;

  static std::vector<uint32_t> exported_pins;
  uint32_t      pin_;
  uint32_t      pin_mask_;
  volatile      uint32_t* data_;       // data register
  Logger&       log_;
  int           fd_;
};
}}}

#endif  // UTILS_IO_ADC_HPP_