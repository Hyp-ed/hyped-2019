#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "propulsion/controller.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::motor_control::Controller;

#define NUM_MOTORS 1

int main(int argc, char** argv) {
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  // Create and initialise motors
  Controller** controllers;
  for (int i = 0; i < NUM_MOTORS; i++) {
    controllers[i] = new Controller(log, i);
  }

  // Register the controllers as nodes on the CANBus
  for (int i = 0; i < NUM_MOTORS; i++) {
    controllers[i]->registerController();
  }

  // Send Configuration messages to controllers
  for (int i = 0; i < NUM_MOTORS; i++) {
    controllers[i]->configure();
  }

  // Send auto align command
  for (int i = 0; i < NUM_MOTORS; i++) {
    controllers[i]->autoAlignMotorPosition();
  }

  // Sleep to process any errors
  Thread::sleep(100000);
}