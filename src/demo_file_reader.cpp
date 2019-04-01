#include "propulsion/file_reader.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "propulsion/controller.hpp"
#include <array>

using hyped::motor_control::FileReader;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::motor_control::ControllerMessage;

int main(int argc, char* argv[]) {
  System::parseArgs(argc, argv);
  Logger log(true, 0);

  std::string messages[16];
  if (FileReader::readFileData(messages, "src/propulsion/configFiles/test_message.txt")) {
    for (int i = 0; i < 16; i++) {
      log.INFO("DEMO",messages[i].c_str());
    }
  }
}
