// #include <stdio.h>
#include "utils/concurrent/thread.hpp"
#include "utils/io/gpio.hpp"
#include "sensors/gpio_counter.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"

// using hyped::utils::concurrent::Thread;
using hyped::utils::io::GPIO;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::sensors::GpioCounter;
using hyped::data::StripeCounter;      // data.hpp
namespace io = hyped::utils::io;

constexpr int kStripeNum = 10;
constexpr uint64_t kCheckTime = 3150000;       //3.15 seconds

int main(int argc, char* argv[]) { 
  System::parseArgs(argc, argv);
  Logger log(true, 0);

  GPIO keyence_pin(67, io::gpio::kIn);           // 4 down on right
  GpioCounter keyence(67);

  int stripe_count = 0;
  Timer timer;
  timer.reset();
  timer.start();
  uint64_t start_time = timer.getMicros();

  while(stripe_count<kStripeNum){
    keyence.run();                        // will it reach next line if waiting for value?
    if(timer.getMicros()-start_time>kCheckTime){      // if time between stripe suceeded
      stripe_count++;
      log.DBG("TEST-KEYENCE", "Missed stripe at time %f",timer.getMicros());
      start_time = timer.getMicros();
    }
    StripeCounter stripe_data = keyence.getStripeCounter();
    stripe_count += stripe_data.count.value;
    log.DBG("TEST-KEYENCE", "Stripe hit at time %f",stripe_data.count.timestamp);
  }

  timer.stop();
  // log.DBG("TEST-KEYENCE", "Message here");
}