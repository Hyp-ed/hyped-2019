#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "propulsion/controller.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::motor_control::Controller;

int8_t readVel(const char* filepath)
{
  FILE* fp;

  fp = fopen(filepath, "r");

  int8_t vel = 0;

  if (fp == NULL) {
  } else {
    char line[255];
    fgets(line, 255, fp);
    vel = std::atoi(line);
  }
  fclose(fp);
  return vel;
}

int main(int argc, char** argv) {
  System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  Controller controller(log, 5);

  controller.registerController();
  controller.configure();
  controller.enterOperational();

  int8_t vel = 0;
  // Thread::sleep(1000);
  // controller.sendTargetVelocity(10);

  // Thread::sleep(100000);

  // while(1) {
  // vel = readVel("data/in/target_velocity_test.txt");

  // controller.sendTargetVelocity(vel);
  // Thread::sleep(1000);
  // }
  
  controller.autoAlignMotorPosition();

  // controller.sendTargetVelocity(50);

  // for (auto i = 0; i < 20; i++) {
  //   controller.updateActualVelocity();
  //   auto vel = controller.getVelocity();
  //   log.INFO("TEST", "Actual velocity: %d rpm", vel);
  //   Thread::sleep(1000);
  // }
  // controller.enterPreOperational();
}