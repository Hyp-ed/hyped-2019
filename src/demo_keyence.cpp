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

  GPIO test_39(39, io::gpio::kOut,log);          // 6 down on left
  log.DBG("TEST-KEYENCE", "Pin created");
  test_39.clear();
  test_39.set();

  GpioCounter keyence(38);        // 4 down on left

  int stripe_count = 0;
  Timer timer;      // get timer started
  timer.reset();
  timer.start();
  Thread::sleep(50);
  uint64_t start_time = timer.getMicros();                      // timer staying at zero!!!!!
  log.DBG("TEST-KEYENCE", "Start time: %f", start_time);

  keyence.start();                        // start gpio_counter thread and run()
  log.DBG("TEST-KEYENCE", "Thread started");

  Thread::sleep(500);
  StripeCounter stripe_data;
  int num = 0;
  while(stripe_count<kStripeNum){             // so all of this runs while it waits
    if(num%2==0){          // to test with gpio pin- I have come to the conclusion that GPIO pins do not help testing for me
      test_39.set();
      log.DBG("TEST-KEYENCE", "Read GPIO pin val %d", test_39.read());
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

    stripe_data = keyence.getStripeCounter();       // to get value     // not reading data
    log.DBG("TEST-KEYENCE", "Stripe_data count: %f", stripe_data.count.value);
    stripe_count += stripe_data.count.value;
    log.INFO("TEST-KEYENCE", "Stripe hit at time %f",stripe_data.count.timestamp);      // 0 because no data!
    Thread::sleep(500);
    num++;
    timer.stop();
    log.DBG("TEST-KEYENCE", "Time: %f", timer.getMicros());
    timer.start();
    log.DBG("TEST-KEYENCE","num: %d", num);

  }

  timer.stop();
  // log.DBG("TEST-KEYENCE", "Message here");
}