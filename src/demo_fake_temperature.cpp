#include "utils/concurrent/thread.hpp"
#include "sensors/fake_temperature.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"
#include "data/data.hpp"

#define FAIL 1

using hyped::utils::concurrent::Thread;
using hyped::sensors::FakeTemperature;
using hyped::utils::Logger;
using hyped::utils::System;
using hyped::data::Data;
using hyped::data::State;

uint8_t kStripeNum = 30;

int main(int argc, char* argv[]) {
  hyped::utils::System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  #if FAIL
  FakeTemperature fake_temperature(log, true);
  #else
  FakeTemperature fake_temperature(log, false);
  #endif
  Data& data = Data::getInstance();

  auto state = data.getStateMachineData();
  state.current_state = State::kAccelerating;
  data.setStateMachineData(state);

  for (int i = 0; i < 50; i++) {
    fake_temperature.run();
    int value = fake_temperature.getData();
    log.DBG("DEMO-FakeTemperature", "Temperature = %d", value);
    Thread::sleep(250);
  }
}