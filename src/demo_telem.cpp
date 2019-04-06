#include <thread>
#include "telemetry/main.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"

using hyped::utils::Logger;
using hyped::utils::System;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;

void loop(Logger& logger);

int main(int argc, char* argv[]) {
    System::parseArgs(argc, argv);
    System& sys = System::getSystem();
    Logger log_tlm(sys.verbose_tlm, sys.debug_tlm);

    std::thread loopThread {loop, std::ref(log_tlm)};

    Thread* telemetry = new hyped::telemetry::Main(4, log_tlm);
    telemetry->start();
    telemetry->join();

    loopThread.join();
}

void loop(Logger& logger) {
    Data& data = Data::getInstance();

    while (true) {
        Navigation nav_data = data.getNavigationData();
        Telemetry telem_data = data.getTelemetryData();

        logger.DBG2("Telemetry", "SHARED module_status: %d", telem_data.module_status);
        logger.DBG2("Telemetry", "SHARED launch_command: %s", telem_data.launch_command ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED reset_command: %s", telem_data.reset_command ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED run_length: %f", telem_data.run_length);
        logger.DBG2("Telemetry", "SHARED service_propulsion_go: %s", telem_data.service_propulsion_go ? "true" : "false"); // NOLINT

        nav_data.distance = 111;
        nav_data.velocity = 111;
        nav_data.acceleration = 111;
        data.setNavigationData(nav_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        nav_data.distance = 222;
        nav_data.velocity = 222;
        nav_data.acceleration = 222;
        data.setNavigationData(nav_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
