#include "utils/concurrent/thread.hpp"
#include <thread>
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "./embrakes/main.hpp"
#include <unistd.h>
#include "utils/io/can.hpp"
#include <iostream>

#include "./embrakes/retractor_manager.hpp"

using hyped::System;
using hyped::utils::Logger;
using hyped::embrakes::Main;

Logger *log_embrakes;


int main(int argc, char *argv[])
{
	// System setup
	hyped::utils::System::parseArgs(argc, argv);

	System &sys = System::getSystem();

	log_embrakes = new Logger(sys.verbose_motor, sys.debug_motor);

	Main *main = new Main(1,*log_embrakes);

    main->start();
	

	while (true)
		;

	return 0;
}