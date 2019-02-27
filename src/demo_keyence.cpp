#include <stdio.h>
#include "utils/concurrent/thread.hpp"
#include "utils/io/gpio.hpp"
#include "sensors/gpio_counter.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"

using hyped::utils::concurrent::Thread;
using hyped::utils::io::GPIO;
using hyped::sensors::GpioCounter;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::data::StripeCounter;      // data.hpp

using namespace hyped::data;
namespace io = hyped::utils::io;

constexpr int kStripeNum = 50;
constexpr uint64_t kCheckTime = 3150000;       //3.15 seconds

int main(int argc, char* argv[]) { 
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);

  GPIO keyence_pin(67, io::gpio::kIn);           // 4 down on right
  GPIO test_68(68, io::gpio::kOut);          // 5 down on right

  GpioCounter keyence(67);

  int stripe_count = 0;
  Timer timer;
  timer.reset();
  timer.start();
  uint64_t start_time = timer.getMicros();
  test_68.clear();
  int num = 0;

  keyence.start();                        // start gpio_counter thread

  while(stripe_count<kStripeNum){
    if(num%2>0){          // to test with gpio pin
      test_68.set();
    }
    else{
      test_68.clear();
    }
    if(timer.getMicros()-start_time>kCheckTime){      // if time between stripe suceeded
      stripe_count++;
      log.DBG("TEST-KEYENCE", "Missed stripe at time %f",timer.getMicros());
      start_time = timer.getMicros();
    }
    StripeCounter stripe_data = keyence.getStripeCounter();       // to get value
    stripe_count += stripe_data.count.value;
    log.DBG("TEST-KEYENCE", "Stripe hit at time %f",stripe_data.count.timestamp);

  }

  timer.stop();
  // log.DBG("TEST-KEYENCE", "Message here");
}