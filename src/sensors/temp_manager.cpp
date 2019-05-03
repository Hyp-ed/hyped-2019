#include "sensors/temp_manager.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"

namespace hyped {

using data::Data;
using data::Sensors;
using utils::System;

namespace sensors {
TempManager::TempManager(Logger& log)
    : ManagerInterface(log),
      sys_(System::getSystem()),
      data_(Data::getInstance())
{}

void TempManager::run() {
  
}

}}  // namespace hyped::sensors 
