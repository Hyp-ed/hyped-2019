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

constexpr int kStripeNum = 15;
constexpr uint64_t kCheckTime = 3150000;       //3.15 seconds

int main(int argc, char* argv[]) { 
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);

  // GPIO test_39(66, io::gpio::kOut,log);          // 6 down on left
  // log.INFO("TEST-KEYENCE", "Pin created");
  // test_39.clear();
  // test_39.set();


  /*
  varying speed with potentiometer at speed w = 2*pi*f

  have code count number of times stripe is detected

  */

  GpioCounter keyence(66);        // 4 down on left

  uint32_t stripe_count = 0;
  uint32_t prev_count = 0;
  Timer timer;      // get timer started
  timer.reset();
  timer.start();
  Thread::sleep(50);
  
  keyence.start();                        // start gpio_counter thread and run()
  uint64_t start_time = timer.getTimeMicros();
  log.INFO("TEST-KEYENCE", "Start time: %f", start_time);
  log.INFO("TEST-KEYENCE", "Thread started");

  Thread::sleep(500);
  StripeCounter current_stripe_data;

  int run_counter = 0;
  while(stripe_count<kStripeNum){             // so all of this runs while it waits
    // log.DBG("TEST-KEYENCE", "Stripes counted: %d",stripe_count);
    
    current_stripe_data = keyence.getStripeCounter();       // to get value
    
    stripe_count = current_stripe_data.count.value;

    if(timer.getTimeMicros()-start_time>kCheckTime&&stripe_count==prev_count){      // too much time && count not changed
      log.INFO("TEST-KEYENCE", "No stripe detected!");  
      // log.INFO("TEST-KEYENCE", "Missed stripe at time %f",timer.getTimeMicros());
      start_time = timer.getTimeMicros();
    }
    else if(stripe_count!=prev_count){      // has hit stripe
      log.INFO("TEST-KEYENCE", "Stripe hit at time %f",current_stripe_data.count.timestamp);      // 0 because no data
      prev_count = stripe_count;
    }

    // log.DBG("TEST-KEYENCE", "Stripe_data count: %f", current_stripe_data.count.value);
    //Thread::sleep(100);
    run_counter++;
    
    log.DBG("TEST-KEYENCE","run_counter: %d", run_counter);

  }
  timer.stop();
  log.DBG("TEST-KEYENCE", "Time: %f", timer.getMillis());     // still output zero
  log.DBG("TEST-KEYENCE", "DONE WITH RUN");
}