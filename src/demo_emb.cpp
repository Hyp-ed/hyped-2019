#include "utils/concurrent/thread.hpp"
#include <thread>
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "./embrakes/main.hpp"
#include "./embrakes/brakes_controller.hpp"
#include "./embrakes/brakes_controller_interface.hpp"
#include <unistd.h>
#include "utils/io/can.hpp"
#include <iostream>

using hyped::System;
using hyped::utils::Logger;
using hyped::embrakes::Main;
using hyped::embrakes::Controller;

int main(int argc, char *argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  Controller contr = Controller(log, 20);

  uint8_t message[8] = {0x00, 0x34, 0x12, 0x01, 0x00, 0x00, 0x00, 0x00};

  contr.sendData(message);

}