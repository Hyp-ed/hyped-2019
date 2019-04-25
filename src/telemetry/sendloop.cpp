/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: April 2019
 * Description:
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "telemetry/sendloop.hpp"
#include "telemetry/utils.hpp"

namespace hyped {
namespace telemetry {

SendLoop::SendLoop(Logger& log, Main* main_pointer)
    : Thread {log},
      main_ref_ {*main_pointer},
      data_ {data::Data::getInstance()}
{
    log_.DBG("Telemetry", "Telemetry SendLoop thread object created");
}

void SendLoop::run()
{
    log_.DBG("Telemetry", "Telemetry SendLoop thread started");

    telemetry_data::ClientToServer msg;

    while (true) {
        packNavigationMessage(msg);
        packStateMachineMessage(msg);
        packMotorsMessage(msg);
        packBatteriesMessage(msg);
        packSensorsMessage(msg);
        packEmergencyBrakesMessage(msg);

        main_ref_.client_.sendData(msg);
        msg.Clear();

        Thread::sleep(100);
    }
}

void SendLoop::packNavigationMessage(telemetry_data::ClientToServer& msg)
{
    nav_data_ = data_.getNavigationData();
    telemetry_data::ClientToServer::Navigation* navigation_msg = msg.mutable_navigation();

    navigation_msg->set_module_status(Utils::moduleStatusEnumConversion(nav_data_.module_status));
    navigation_msg->set_distance(nav_data_.distance);
    navigation_msg->set_velocity(nav_data_.velocity);
    navigation_msg->set_acceleration(nav_data_.acceleration);
}

void SendLoop::packStateMachineMessage(telemetry_data::ClientToServer& msg)
{
    sm_data_ = data_.getStateMachineData();
    telemetry_data::ClientToServer::StateMachine* state_machine_msg = msg.mutable_state_machine();

    state_machine_msg->set_current_state(Utils::stateEnumConversion(sm_data_.current_state));
}

void SendLoop::packMotorsMessage(telemetry_data::ClientToServer& msg)
{
    motor_data_ = data_.getMotorData();
    telemetry_data::ClientToServer::Motors* motors_msg = msg.mutable_motors();

    motors_msg->set_module_status(Utils::moduleStatusEnumConversion(motor_data_.module_status));
    motors_msg->set_velocity_1(motor_data_.velocity_1);
    motors_msg->set_velocity_2(motor_data_.velocity_2);
    motors_msg->set_velocity_3(motor_data_.velocity_3);
    motors_msg->set_velocity_4(motor_data_.velocity_4);
    motors_msg->set_velocity_5(motor_data_.velocity_5);
    motors_msg->set_velocity_6(motor_data_.velocity_6);
}

void SendLoop::packBatteriesMessage(telemetry_data::ClientToServer& msg)
{
    batteries_data_ = data_.getBatteriesData();
    telemetry_data::ClientToServer::Batteries* batteries_msg = msg.mutable_batteries();

    batteries_msg->set_module_status(Utils::moduleStatusEnumConversion(batteries_data_.module_status)); // NOLINT

    packLpBatteryDataMessage(*batteries_msg, batteries_data_.low_power_batteries);
    packHpBatteryDataMessage(*batteries_msg, batteries_data_.high_power_batteries);
}

template<std::size_t SIZE>
void SendLoop::packLpBatteryDataMessage(batteriesMsg& batteries_msg, std::array<data::BatteryData, SIZE>& battery_data_array) // NOLINT
{
    for (auto battery_data : battery_data_array) {
        telemetry_data::ClientToServer::Batteries::BatteryData* battery_data_msg = batteries_msg.add_low_power_batteries(); // NOLINT
        packBatteryDataMessageHelper(*battery_data_msg, battery_data);
    }
}

template<std::size_t SIZE>
void SendLoop::packHpBatteryDataMessage(batteriesMsg& batteries_msg, std::array<data::BatteryData, SIZE>& battery_data_array) // NOLINT
{
    for (auto battery_data : battery_data_array) {
        telemetry_data::ClientToServer::Batteries::BatteryData* battery_data_msg = batteries_msg.add_high_power_batteries(); // NOLINT
        packBatteryDataMessageHelper(*battery_data_msg, battery_data);
    }
}

void SendLoop::packBatteryDataMessageHelper(batteriesMsg::BatteryData& battery_data_msg, data::BatteryData& battery_data) // NOLINT
{
    battery_data_msg.set_voltage(battery_data.voltage);
    battery_data_msg.set_current(battery_data.current);
    battery_data_msg.set_charge(battery_data.charge);
    battery_data_msg.set_temperature(battery_data.temperature);
    battery_data_msg.set_low_voltage_cell(battery_data.low_voltage_cell);
    battery_data_msg.set_high_voltage_cell(battery_data.high_voltage_cell);
}

void SendLoop::packSensorsMessage(telemetry_data::ClientToServer& msg)
{
    sensors_data_ = data_.getSensorsData();
    telemetry_data::ClientToServer::Sensors* sensors_msg = msg.mutable_sensors();

    sensors_msg->set_module_status(Utils::moduleStatusEnumConversion(sensors_data_.module_status));

    for (data::ImuData imu_data : sensors_data_.imu.value) {
        telemetry_data::ClientToServer::Sensors::ImuData* imu_data_msg = sensors_msg->add_imu();

        imu_data_msg->set_operational(imu_data.operational);

        // hardcoded atm to loop three times bc hyped vector doesn't have a method for vector length
        // or have an iterator to support a ranged for loop
        for (int i = 0; i < 3; i++) {
            imu_data_msg->add_acc(imu_data.acc[i]);
        }
    }
}

void SendLoop::packEmergencyBrakesMessage(telemetry_data::ClientToServer& msg)
{
    emergency_brakes_data_ = data_.getEmergencyBrakesData();
    telemetry_data::ClientToServer::EmergencyBrakes* emergency_brakes_msg = msg.mutable_emergency_brakes(); // NOLINT

    emergency_brakes_msg->set_front_brakes(emergency_brakes_data_.front_brakes);
    emergency_brakes_msg->set_rear_brakes(emergency_brakes_data_.rear_brakes);
}

}  // namespace telemetry
}  // namespace hyped
