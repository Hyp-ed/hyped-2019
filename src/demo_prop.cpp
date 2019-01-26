#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "./propulsion/main.hpp"


using hyped::utils::concurrent::Thread;
using hyped::utils::System;
using hyped::utils::Logger;

int main(int argc, char* argv[]) {

	// System setup
  	hyped::utils::System::parseArgs(argc, argv);

	System& sys = System::getSystem();

	Logger log_motor(sys.verbose_motor, sys.debug_motor);

	Thread* main = new hyped::motor_control::Main(1,log_motor);

	main->start();

	while(true);

	return 0;
}