#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "propulsion/controller.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::motor_control::Controller;

int main(int argc, char** argv) {
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  Controller controller(log, 2);

  controller.registerController();
  controller.configure();
  controller.enterOperational();

  int32_t max_velocity = 200;
  int32_t current_velocity;
  int32_t target_velocity;

  while (controller.getVelocity() < max_velocity) {
    current_velocity = controller.getVelocity();
    target_velocity = current_velocity + 100;
    controller.sendTargetVelocity(target_velocity);
    log.INFO("MOT-TEST", "Current velocity: %d rpm", current_velocity);
    Thread::sleep(1000);
  }

  Thread::sleep(10000);
  controller.enterPreOperational();
}