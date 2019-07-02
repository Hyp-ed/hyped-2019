// demo file to spin up the motors by running the main propulsion thread.

#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "state_machine/hyped-machine.hpp"
#include "propulsion/main.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using hyped::data::StateMachine;
using hyped::data::Motors;
using hyped::data::ModuleStatus;
using hyped::state_machine::HypedMachine;
using hyped::data::Data;
using hyped::data::State;
using hyped::motor_control::Main;

int main(int argc, char** argv)
{
  System::parseArgs(argc, argv);
  Logger& log = System::getLogger();
  Data& data = Data::getInstance();
  StateMachine state = data.getStateMachineData();
  Motors motor_data = data.getMotorData();

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
    state.current_state = State::kAccelerating;
    data.setStateMachineData(state);
  }


  motors->join();
}
