#ifndef SENSORS_TEMPERATURE_HPP_
#define SENSORS_TEMPERATURE_HPP_

#include <cstdint>

#include "data/data.hpp"
#include "sensors/interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

namespace hyped {

using utils::Logger;
using utils::concurrent::Thread;
using utils::System;

namespace sensors {

class Temperature: public AdcInterface, public Thread {
 public:
  Temperature(utils::Logger& log, int pin);
  ~Temperature() {}
  data::TemperatureData getAnalogRead() override;
  void run() override;

 private:
  int scaleData(int voltage);
  int pin_;
  System& sys_;
  utils::Logger& log_;
  data::TemperatureData temp_;
};

}}


#endif  // SENSORS_TEMPERATURE_HPP_