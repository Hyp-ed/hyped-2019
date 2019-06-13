#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/concurrent/thread.hpp"
#include "propulsion/controller.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::motor_control::Controller;

int main(int argc, char** argv) {
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  int node_id = 2;

  Controller controller(log, node_id);

  controller.registerController();
  // correct data needs to be configured in the controller config files:
  controller.configure();
  controller.enterOperational();
  controller.sendTargetVelocity(200);

  for (int i = 0; i < 15; i++) {
    controller.updateActualVelocity();
    uint32_t vel = controller.getVelocity();
    log.INFO("TEST-CONTROLLER", "Actual motor velocity: %d rpm",vel);
    Thread::sleep(1000);
  }
}
