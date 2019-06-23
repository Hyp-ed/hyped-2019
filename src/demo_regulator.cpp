#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "propulsion/RPM_regulator.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::motor_control::RPM_Regulator;

int main(int argc, char** argv)
{
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  RPM_Regulator regulator(log);
  regulator.calculateRPM(0, 0, 0);
  return 0;
}
