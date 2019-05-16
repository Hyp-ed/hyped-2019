#include "sensors/temp_manager.hpp"
#include "sensors/temperature.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"

namespace hyped {

using data::Data;
using data::Sensors;
using utils::System;

namespace sensors {
TempManager::TempManager(Logger& log)
    : TempManagerInterface(log),
      sys_(System::getSystem()),
      data_(Data::getInstance()),
      analog_pins_ {0, 1, 2 , 3}
{
  for (int i = 0; i < data::Sensors::kNumThermistors; i++) {    // creates new real objects
    temp_[i] = new Temperature(log, analog_pins_[i]);
  }
  log_.INFO("TEMP-MANAGER", "temp data has been initialised");
}

void TempManager::run() {
  // need to average temperature and return value uint8_t to data struct

  // TODO(Greg): might remove implementation of DataArray sensors_temp_
  uint8_t average = 0;
  for (int i = 0; i < data::Sensors::kNumThermistors; i++) {
    // average += temp_[i]->getAnalogRead().temp.value;
    sensors_temp_.value[i] = temp_[i]->getAnalogRead();   // wrong syntax, needs to be reference
  }
  sensors_temp_.timestamp = utils::Timer::getTimeMicros();
  // data_.setTemperature(sensors_temp_);
}

}}  // namespace hyped::sensors 