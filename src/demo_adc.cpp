
// #include <stdio.h>
#include "utils/concurrent/thread.hpp"
#include "utils/io/adc.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"

// using hyped::utils::concurrent::Thread;
using hyped::utils::io::ADC;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::utils::concurrent::Thread;
namespace io = hyped::utils::io;

int main(int argc, char* argv[]) { 
  System::parseArgs(argc, argv);
  Logger log(true, 0);
  
  ADC analog(0, log);
  Thread::sleep(100);

  for (int i=0; i<20; i++) {
    log.INFO("DEMO-ADC", "%d", analog.read());
    Thread::sleep(200);
  }
  
}