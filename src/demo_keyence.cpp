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

  GPIO test_39(39, io::gpio::kOut,log);          // 6 down on left
  log.DBG("TEST-KEYENCE", "Pin created");
  test_39.clear();
  test_39.set();

  GpioCounter keyence(38);        // 4 down on left

  uint32_t stripe_count = 0;
  uint32_t prev_count = -1;
  Timer timer;      // get timer started
  timer.reset();
  timer.start();
  Thread::sleep(50);
  uint64_t start_time = timer.getMicros();                      // timer staying at zero!!!!!
  log.DBG("TEST-KEYENCE", "Start time: %f", start_time);

  keyence.start();                        // start gpio_counter thread and run()
  log.DBG("TEST-KEYENCE", "Thread started");

  Thread::sleep(500);
  StripeCounter current_stripe_data;

  int run_counter = 0;
  while(stripe_count<kStripeNum){             // so all of this runs while it waits
    if(run_counter%2==0){          // to test with gpio pin- I have come to the conclusion that GPIO pins do not help testing for me
      test_39.set();
      log.DBG("TEST-KEYENCE", "Read GPIO pin val %d", test_39.wait());
    }
    else{
      test_39.clear();
    }
    log.DBG("TEST-KEYENCE", "Stripes counted: %d",stripe_count);

    // if(timer.getMicros()-start_time>kCheckTime){      // if time between stripe suceeded
    //   stripe_count++;
    //   log.INFO("TEST-KEYENCE", "Missed stripe at time %f",timer.getMicros());
    //   start_time = timer.getMicros();
    // }

    current_stripe_data = keyence.getStripeCounter();       // to get value     // not reading data!!
    stripe_count = current_stripe_data.count.value;
    if(stripe_count==prev_count){     // same value as last time
      // no stripe detected
      log.INFO("TEST-KEYENCE", "No stripe detected!");  
    }
    else{
      // has hit stripe
      log.INFO("TEST-KEYENCE", "Stripe hit at time %f",current_stripe_data.count.timestamp);      // 0 because no data
      prev_count = stripe_count;
    }

    log.DBG("TEST-KEYENCE", "Stripe_data count: %f", current_stripe_data.count.value);
    Thread::sleep(100);
    run_counter++;
    // timer.stop();
    // log.DBG("TEST-KEYENCE", "Time: %f", timer.getMicros());
    // timer.start();
    log.DBG("TEST-KEYENCE","run_counter: %d", run_counter);

  }

  timer.stop();
  // log.DBG("TEST-KEYENCE", "Message here");
}