
#include "sensors/fake_imu.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "data/data.hpp"

#define FAIL 1

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::data::NavigationType;
using hyped::data::NavigationVector;
using hyped::data::StateMachine;
using hyped::ImuData;
using hyped::data::Data;
using hyped::sensors::FakeImuFromFile;
using hyped::utils::concurrent::Thread;
using hyped::data::State;

int main(int argc, char *argv[]) {
  hyped::utils::System::parseArgs(argc, argv);
  Logger log = System::getLogger();
  Data& data = Data::getInstance();
  StateMachine state_machine = data.getStateMachineData();

  state_machine.current_state = State::kAccelerating;
  data.setStateMachineData(state_machine);

  std::string acc_file_path, dec_file_path, em_file_path;
  // Add the files to read bellow:
  acc_file_path = "data/in/acc_state.txt"; 
  dec_file_path = "data/in/decel_state.txt";
  em_file_path = "data/in/decel_state.txt";

  ImuData imu;

  #if FAIL
  FakeImuFromFile fake_imu(log, acc_file_path, dec_file_path, em_file_path, true, false);
  #else
  FakeImuFromFile fake_imu(log, acc_file_path, dec_file_path, em_file_path, false, false);
  #endif
  for (int i = 0; i < 50; i++) {
    fake_imu.getData(&imu);
    NavigationVector accData = imu.acc;
    log.INFO("IMU_DATA", "Acc: x:%2.5f, y:%2.5f, z:%2.5f", accData[0], accData[1], accData[2]);
    Thread::sleep(50); 
  }
}
