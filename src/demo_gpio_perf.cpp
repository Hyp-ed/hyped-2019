
// #include <stdio.h>
#include "utils/concurrent/thread.hpp"
#include "utils/io/gpio.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"

// using hyped::utils::concurrent::Thread;
using hyped::utils::io::GPIO;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::Timer;
namespace io = hyped::utils::io;

// use this demo file to test the gpio performance between pin masking method and file system method
// in gpio.cpp change GPIOFS
int main(int argc, char* argv[]) { 
  System::parseArgs(argc, argv);
  Logger log(true, 0);

  GPIO pin_38(38, io::gpio::kOut);
  GPIO pin_39(39, io::gpio::kIn);

  Timer timer;
  timer.reset();
  timer.start();
  for (int i = 0; i < 1000000; i++) {     // create cycle of set and clear (0 to 1)
    pin_38.clear();
    pin_38.set();
  }
  timer.stop();
  log.DBG("TEST-TIMER", "Time for 1 million cycles: %f", timer.getMillis());
}