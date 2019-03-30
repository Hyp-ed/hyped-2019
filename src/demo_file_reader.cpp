#include "propulsion/file_reader.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

using hyped::motor_control::FileReader;
using hyped::utils::System;
using hyped::utils::Logger;

int main(int argc, char* argv[]) {
  System::parseArgs(argc, argv);
  Logger& log = System::getLogger();

  FileReader reader(log);
  const char* logger_message = "Data in message: %d %d %d %d %d %d %d %d";
  uint8_t test_message_template_[8];
  reader.readFileData(test_message_template_, 8);
  log.INFO("MOTOR", logger_message,test_message_template_[0],
              test_message_template_[0],test_message_template_[0],test_message_template_[0],
              test_message_template_[0],test_message_template_[0],test_message_template_[0],
              test_message_template_[0]);
}
