// demo file to spin up the motors by running the main propulsion thread.

#include <stdio.h>

#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "propulsion/main.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::data::StateMachine;
using hyped::data::Motors;
using hyped::data::ModuleStatus;
<<<<<<< Updated upstream
=======
using hyped::data::Navigation;
using hyped::data::NavigationType;
using hyped::state_machine::HypedMachine;
>>>>>>> Stashed changes
using hyped::data::Data;
using hyped::data::State;
using hyped::data::Navigation;
using hyped::motor_control::Main;

NavigationType readVel(char* filepath);

#define VEL

int main(int argc, char** argv)
{
  System::parseArgs(argc, argv);
  Logger& log = System::getLogger();
  Data& data = Data::getInstance();
  StateMachine state = data.getStateMachineData();
  Motors motor_data = data.getMotorData();
  Navigation nav_data = data.getNavigationData();

  state.current_state = State::kIdle;
  data.setStateMachineData(state);

  Main* motors = new Main(1, log);
  motors->start();

  while (motor_data.module_status != ModuleStatus::kInit) {
    motor_data = data.getMotorData();
  }
  if (motor_data.module_status == ModuleStatus::kInit) {
    state.current_state = State::kCalibrating;
    data.setStateMachineData(state);
  }

  while (motor_data.module_status != ModuleStatus::kReady) {
    motor_data = data.getMotorData();
  }
  if (motor_data.module_status == ModuleStatus::kReady) {
    char ch;
    puts("Waiting to accelerate. Press ENTER to continue.");
    scanf("%c", &ch);

    state.current_state = State::kAccelerating;
    data.setStateMachineData(state);
    log.INFO("TEST", "Accelerating");
  }

  while (1) {
    Navigation nav_data = data.getNavigationData();
    nav_data.velocity = readVel("data/in/acceleration_profile.txt");
    data.setNavigationData(nav_data);
  }

  motors->join();
}

NavigationType readVel(char* filepath) {
  FILE* fp;
  fopen(filepath, "r");

  if (fp == NULL) {
    return 0;
  }
}