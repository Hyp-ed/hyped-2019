#include "utils/concurrent/thread.hpp"
#include "sensors/fake_batteries.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"
#include "data/data.hpp"

#define FAIL 1

using hyped::utils::concurrent::Thread;
using hyped::sensors::FakeBatteries;
using hyped::utils::Logger;
using hyped::utils::System;
using hyped::data::Data;
using hyped::data::State;
using hyped::BatteryData;

uint8_t kStripeNum = 30;

int main(int argc, char* argv[]) {
  hyped::utils::System::parseArgs(argc, argv);
  Logger log = System::getLogger();

  #if FAIL
  FakeBatteries fake_batteries(log, true, true);
  #else
  FakeBatteries fake_batteries(log, true, false);
  #endif
  Data& data = Data::getInstance();

  auto state = data.getStateMachineData();
  state.current_state = State::kAccelerating;
  data.setStateMachineData(state);

  BatteryData battery_data;

  for (int i = 0; i < 50; i++) {
    fake_batteries.getData(&battery_data);
    log.DBG("DEMO-FakeBatteries", "Voltage = %d, Current = %d, Charge = %d, Temperature = %d",
            battery_data.voltage, battery_data.current,
            battery_data.charge, battery_data.temperature);
    Thread::sleep(250);
  }
}