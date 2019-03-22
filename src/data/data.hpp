/*
 * Organisation: HYPED
 * Date:
 * Description: Class for data exchange between sub-team threads and structures
 * for holding data produced by each of the sub-teams.
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#ifndef DATA_DATA_HPP_
#define DATA_DATA_HPP_

#include <cstdint>
#include <array>
#include "utils/math/vector.hpp"
#include "data/data_point.hpp"
#include "utils/concurrent/lock.hpp"

using std::array;

namespace hyped {

// imports
using utils::math::Vector;
using utils::concurrent::Lock;

namespace data {

// -------------------------------------------------------------------------------------------------
// Global Module States
// -------------------------------------------------------------------------------------------------
enum class ModuleStatus {
  kStart,   // Initial module state
  kInit,  // SM transistions to Calibrating if all modules have Init status.
  kReady,  // SM transistions to Ready if Motors and Navigation have the Ready status.
  kCriticalFailure  // SM transitions to EmergencyBraking/FailureStopped
};

struct Module {
  ModuleStatus module_status = ModuleStatus::kStart;
};

// -------------------------------------------------------------------------------------------------
// Navigation
// -------------------------------------------------------------------------------------------------
typedef float NavigationType;
struct Navigation : public Module {
  NavigationType  distance;  // m
  NavigationType  velocity;  // m/s
  NavigationType  acceleration;  // m/s^2
  NavigationType  emergency_braking_distance;  // m
  NavigationType  braking_distance = 750;  // m
};

// -------------------------------------------------------------------------------------------------
// Raw Sensor data
// -------------------------------------------------------------------------------------------------
struct Sensor {
  bool operational;
};

typedef Vector<NavigationType, 3> NavigationVector;
struct ImuData : public Sensor {
  NavigationVector acc;
};

struct StripeCounter : public Sensor {
  DataPoint<uint32_t> count;
};

struct Sensors : public Module {
  static constexpr int kNumImus = 8;
  static constexpr int kNumKeyence = 2;

  DataPoint<array<ImuData, kNumImus>> imu;
  array<StripeCounter, kNumKeyence>  keyence_stripe_counter;
};

struct SensorCalibration {
  array<NavigationVector, Sensors::kNumImus> imu_variance;
};

struct Battery {
  uint16_t  voltage;  // V
  int16_t   current;  // mA
  uint8_t   charge;
  int8_t    temperature;  // C
  uint16_t  low_voltage_cell;  // V
  uint16_t  high_voltage_cell;  // V
};

struct Batteries : public Module {
  static constexpr int kNumLPBatteries = 2;
  static constexpr int kNumHPBatteries = 3;

  array<Battery, kNumLPBatteries> low_power_batteries;
  array<Battery, kNumHPBatteries> high_power_batteries;
};

struct EmergencyBrakes : public Module {
  bool front_brakes;       // true if front facing emergency brakes deploy
  bool rear_brakes;      // true if rear facing emergency brakes deploy
};

// -------------------------------------------------------------------------------------------------
// Motor data
// -------------------------------------------------------------------------------------------------

struct Motors : public Module {
  int32_t velocity_1;
  int32_t velocity_2;
  int32_t velocity_3;
  int32_t velocity_4;
  int32_t velocity_5;
  int32_t velocity_6;
};

// -------------------------------------------------------------------------------------------------
// Communications data
// -------------------------------------------------------------------------------------------------

struct Communications : public Module {
  bool launch_command;
  bool reset_command;
  float run_length;  // m
  bool service_propulsion_go;
};

// -------------------------------------------------------------------------------------------------
// State Machine States
// -------------------------------------------------------------------------------------------------
enum State {
  kIdle,
  kCalibrating,
  kReady,
  kAccelerating,
  kNominalBraking,
  kEmergencyBraking,
  kRunComplete,
  kFailureStopped,
  kExiting,
  kFinished,
  kInvalid,
  num_states
};

extern const char* states[num_states];

struct StateMachine {
  bool critical_failure;
  State current_state;
};

// -------------------------------------------------------------------------------------------------
// Common Data structure/class
// -------------------------------------------------------------------------------------------------
/**
 * @brief      A singleton class managing the data exchange between sub-team
 * threads.
 */
class Data {
 public:
  /**
   * @brief      Always returns a reference to the only instance of `Data`.
   */
  static Data& getInstance();

  /**
   * @brief      Retrieves data related to the state machine. Data has high priority.
   */
  StateMachine getStateMachineData();

  /**
   * @brief      Should be called by state machine team to update data.
   */
  void setStateMachineData(const StateMachine& sm_data);

  /**
   * @brief      Retrieves data produced by navigation sub-team.
   */
  Navigation getNavigationData();

  /**
   * @brief      Should be called by navigation sub-team whenever they have new data.
   */
  void setNavigationData(const Navigation& nav_data);

  /**
   * @brief Get the Temperature from the IMU
   *
   * @return int temperature in degrees C
   */
  int getTemperature();

  /**
   * @brief Set the Temperature from the IMU
   *
   * @param temp - temp in degrees C
   */
  void setTemperature(int temp);

  /**
   * @brief      Retrieves data from all sensors
   */
  Sensors getSensorsData();

  /**
   * @brief      Should be called to update sensor data.
   */
  void setSensorsData(const Sensors& sensors_data);
  /**
   * @brief      Should be called to update sensor imu data.
   */
  void setSensorsImuData(const DataPoint<array<ImuData, Sensors::kNumImus>>& imu);
  /**
   * @brief      Should be called to update sensor keyence data.
   */
  void setSensorsKeyenceData(const array<StripeCounter, Sensors::kNumKeyence>&  keyence_stripe_counter);
  /**
   * @brief      Should be called to update sensor calibration data
   */
  void setCalibrationData(const SensorCalibration sensor_calibration_data);
  /**
   * @brief      Retrieves data from the calibrated sensors
   */
  SensorCalibration getCalibrationData();

  /**
   * @brief      Retrieves data from the batteries.
   */
  Batteries getBatteriesData();

  /**
   * @brief      Should be called to update battery data
   */
  void setBatteryData(const Batteries& batteries_data);

  /**
   * @brief      Retrieves data from the emergency brakes.
   */
  EmergencyBrakes getEmergencyBrakesData();

  /**
   * @brief      Should be called to update emergency brakes data
   */
  void setEmergencyBrakesData(const EmergencyBrakes& emergency_brakes_data);

  /**
   * @brief      Retrieves data produced by each of the four motors.
   */
  Motors getMotorData();

  /**
   * @brief      Should be called to update motor data.
   */
  void setMotorData(const Motors& motor_data);

  /**
   * @brief      Retrieves data on whether stop/kill power commands have been issued.
   */
  Communications getCommunicationsData();

  /**
   * @brief      Should be called to update communications data.
   */
  void setCommunicationsData(const Communications& communications_data);

 private:
  StateMachine state_machine_;
  Navigation navigation_;
  Sensors sensors_;
  Motors motors_;
  Batteries batteries_;
  Communications communications_;
  SensorCalibration calibration_data_;
  EmergencyBrakes emergency_brakes_;
  int temperature_;  // In degrees C


  // locks for data substructures
  Lock lock_state_machine_;
  Lock lock_navigation_;
  Lock lock_sensors_;
  Lock lock_motors_;
  Lock lock_temp_;

  Lock lock_communications_;
  Lock lock_batteries_;
  Lock lock_emergency_brakes_;
  Lock lock_calibration_data_;
};

}}  // namespace hyped::data

#endif  // DATA_DATA_HPP_
