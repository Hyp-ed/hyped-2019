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
    delete telemetry;

    loopThread.join();
}

void loop(Logger& logger) {
    Data& data = Data::getInstance();

    while (true) {
        Navigation nav_data                     = data.getNavigationData();
        StateMachine sm_data                    = data.getStateMachineData();
        Motors motor_data                       = data.getMotorData();
        EmergencyBrakes emergency_brakes_data   = data.getEmergencyBrakesData();
        Batteries batteries_data                = data.getBatteriesData();
        Telemetry telem_data                    = data.getTelemetryData();

        logger.DBG2("Telemetry", "SHARED module_status: %d", telem_data.module_status);
        logger.DBG2("Telemetry", "SHARED launch_command: %s", telem_data.launch_command ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED reset_command: %s", telem_data.reset_command ? "true" : "false"); // NOLINT
        logger.DBG2("Telemetry", "SHARED run_length: %f", telem_data.run_length);
        logger.DBG2("Telemetry", "SHARED service_propulsion_go: %s", telem_data.service_propulsion_go ? "true" : "false"); // NOLINT

        nav_data.module_status = ModuleStatus::kReady;
        nav_data.distance = 111;
        nav_data.velocity = 111;
        nav_data.acceleration = 111;
        data.setNavigationData(nav_data);

        sm_data.current_state = kReady;
        data.setStateMachineData(sm_data);

        motor_data.velocity_1 = 101;
        motor_data.velocity_2 = 102;
        data.setMotorData(motor_data);

        emergency_brakes_data.front_brakes = false;
        emergency_brakes_data.rear_brakes = false;
        data.setEmergencyBrakesData(emergency_brakes_data);

        BatteryData low_power;
        low_power.voltage = 16;
        low_power.current = -16;
        low_power.charge = 16;
        low_power.temperature = -16;
        low_power.low_voltage_cell = 16;
        low_power.high_voltage_cell = 16;

        BatteryData high_power;
        high_power.voltage = 10;
        high_power.current = -10;
        high_power.charge = 10;
        high_power.temperature = -10;
        high_power.low_voltage_cell = 10;
        high_power.high_voltage_cell = 10;

        batteries_data.low_power_batteries.at(0) = low_power;
        batteries_data.high_power_batteries.at(0) = high_power;
        data.setBatteriesData(batteries_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        nav_data.module_status = ModuleStatus::kInit;
        nav_data.distance = 222;
        nav_data.velocity = 222;
        nav_data.acceleration = 222;
        data.setNavigationData(nav_data);

        sm_data.current_state = kCalibrating;
        data.setStateMachineData(sm_data);

        motor_data.velocity_1 = 201;
        motor_data.velocity_2 = 202;
        data.setMotorData(motor_data);

        emergency_brakes_data.front_brakes = true;
        emergency_brakes_data.rear_brakes = true;
        data.setEmergencyBrakesData(emergency_brakes_data);

        low_power.voltage = 26;
        low_power.current = -26;
        low_power.charge = 26;
        low_power.temperature = -26;
        low_power.low_voltage_cell = 26;
        low_power.high_voltage_cell = 26;

        high_power.voltage = 20;
        high_power.current = -20;
        high_power.charge = 20;
        high_power.temperature = -20;
        high_power.low_voltage_cell = 20;
        high_power.high_voltage_cell = 20;

        batteries_data.low_power_batteries.at(0) = low_power;
        batteries_data.high_power_batteries.at(0) = high_power;
        data.setBatteriesData(batteries_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
