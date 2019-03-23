#include "telemetry/main.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

using hyped::utils::Logger;
using hyped::utils::System;
using hyped::utils::concurrent::Thread;

int main(int argc, char* argv[]) {
    System::parseArgs(argc, argv);
    System& sys = System::getSystem();
    Logger log_tlm(sys.verbose_tlm, sys.debug_tlm);

    Thread* telemetry = new hyped::communications::Main(4, log_tlm);
    telemetry->start();
    telemetry->join();
}
