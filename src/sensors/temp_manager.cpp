
// #include "sensors/temp_manager.hpp"
// #include "sensors/temperature.hpp"
// #include "data/data.hpp"
// #include "utils/timer.hpp"

// namespace hyped {

// using data::Data;
// using data::Sensors;
// using utils::System;

// namespace sensors {
// TempManager::TempManager(Logger& log)
//     : log_(log),
//       sys_(System::getSystem()),
//       data_(Data::getInstance()),
//       analog_pins_ {1}
// {
//   for (int i = 0; i < kNumThermistors; i++) {    // creates new real objects
//     Temperature* temp = new Temperature(log, analog_pins_[i]);
//     temp_[i] = temp;
//   }
//   log_.INFO("TEMP-MANAGER", "temp data has been initialised");
// }

// void TempManager::runTemperature()
// {
//   while (sys_.running_) {
//     int average = 0;
//     for (int i = 0; i < kNumThermistors; i++) {
//       average += temp_[i]->getTemperature();
//       log_.DBG1("TEMP-MANAGER", "Sum: %d", average);
//     }
//     average = round(average/kNumThermistors);
//     log_.DBG1("TEMP-MANAGER", "Average after rounding: %d", average);
//     pod_temp_.temp = average;
//     log_.DBG1("TEMP-MANAGER", "pod_temp_: %d", pod_temp_.temp);

//     // // check ambient temperature
//     // if (pod_temp_.module_status != data::ModuleStatus::kCriticalFailure) {
//     //   if (!temperatureInRange()) {
//     //     log_.ERR("TEMP-MANAGER", "temperature spike detected");
//     //     pod_temp_.module_status = data::ModuleStatus::kCriticalFailure;
//     //   }
//     // }
//     // data_.setTemperature(pod_temp_);
//   }
// }

// bool TempManager::temperatureInRange()    // TODO(Anyone): add true temperature range
// {
//   auto& temperature = pod_temp_.temp;
//   log_.DBG1("TEMP-MANAGER", "Temperature from data struct: %d", temperature);
//   if (temperature < -10 || temperature > 50) {  // temperature in -10C to 50C
//     log_.ERR("TEMP-MANAGER", "Temperature out of range: %d", temperature);
//     return false;
//   }
// }

// int TempManager::getAverage(int value) {
//   return value;
// }

// }}  // namespace hyped::sensors
