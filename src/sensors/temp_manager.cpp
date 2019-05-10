// #include "sensors/temp_manager.hpp"
// #include "data/data.hpp"
// #include "utils/timer.hpp"
// #include "utils/io/ain.hpp"

// namespace hyped {

// using data::Data;
// using data::Sensors;
// using utils::System;
// using utils::io::AIN;

// namespace sensors {
// TempManager::TempManager(Logger& log)
//     : sys_(System::getSystem()),
//       data_(Data::getInstance()),
//       log_(log)
// {
//   for (int i = 0; i < data::Sensors::kNumThermistors; i++) {
//     AIN* thermistor = new AIN(i, log_);
//     temp_[i] = thermistor;
//   }
//   log_.INFO("TEMP-MANAGER", "temp data has been initialised");
// }

// void TempManager::run() {
//   // need to average temperature and return value uint8_t to data struct
//   uint8_t average = 0;
//   for (int i = 0; i < data::Sensors::kNumThermistors; i++) {
//     average += temp_[i]->read();
//   }
//   data_.setTemperature(average);
// }

// }}  // namespace hyped::sensors 