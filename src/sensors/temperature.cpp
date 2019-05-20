#include <stdio.h>
#include "utils/concurrent/thread.hpp"
#include "utils/io/adc.hpp"
#include "utils/timer.hpp"
#include "sensors/temperature.hpp"

namespace hyped {

using data::Data;
using data::TemperatureData;
using utils::concurrent::Thread;
using utils::io::ADC;
using hyped::utils::Logger;

namespace sensors {

Temperature::Temperature(utils::Logger& log, int pin)
     : pin_(pin),
       sys_(utils::System::getSystem()),
       log_(log)
{}

void Temperature::run()
{
  ADC thepin(pin_);
  temp_.temp.value = 0;
  temp_.temp.timestamp = utils::Timer::getTimeMicros();

  while (sys_.running_) {
    int raw_value = thepin.read();
    log_.DBG1("THERMISTOR-TEST", "Raw Data: %d", raw_value);
    temp_.temp.value = scaleData(raw_value);
    temp_.temp.timestamp = utils::Timer::getTimeMicros();
    temp_.operational = true;
  }
}

int Temperature::scaleData(int raw_value)
{
  return ((raw_value*(180/255)) - 55);
}

TemperatureData Temperature::getAnalogRead()
{
  return temp_;
}
}}  // namespace hyped::sensors