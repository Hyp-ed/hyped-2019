/*
 * Author: Neil McBlane, Brano Pilnan
 * Organisation: HYPED
 * Date: 05/04/2019
 * Description: Main file for navigation class.
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
#include <algorithm>

#include "navigation/navigation.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/timer.hpp"

namespace hyped {

using hyped::utils::concurrent::Thread;

namespace navigation {

Navigation::Navigation(Logger& log, unsigned int axis/*=0*/)
         : log_(log),
           data_(Data::getInstance()),
           status_(ModuleStatus::kStart),
           counter_(0),
           axis_(axis),
           calibration_limits_({0.001, 0.001, 0.05}),
           acceleration_(0., 0.),
           velocity_(0., 0.),
           distance_(0., 0.),
           acceleration_integrator_(&velocity_),
           velocity_integrator_(&distance_)
{
  log_.INFO("NAV", "Navigation module started");
  for (unsigned int i = 0; i < data::Sensors::kNumImus; i++) {
    filters_[i] = KalmanFilter(1, 1);
    filters_[i].setup();
  }
  status_ = ModuleStatus::kInit;
  updateData();
  log_.INFO("NAV", "Navigation module initialised");
}

ModuleStatus Navigation::getModuleStatus() const
{
  return status_;
}

// TODO(Neil/Lukas/Justus): do this more smartly?
NavigationType Navigation::getAcceleration() const
{
  return acceleration_.value;
}

// TODO(Neil/Lukas/Justus): do this more smartly?
NavigationType Navigation::getVelocity() const
{
  return velocity_.value;
}

// TODO(Neil/Lukas/Justus): do this more smartly?
NavigationType Navigation::getDistance() const
{
  return distance_.value;
}

NavigationType Navigation::getEmergencyBrakingDistance() const
{
  // TODO(Neil): Account for actuation delay and/or communication latency?
  return getVelocity()*getVelocity() / (2*kEmergencyDeceleration);
}

// TODO(Neil): check this is still the same, why is it so?
NavigationType Navigation::getBrakingDistance() const
{
  // A polynomial fit for the braking distance at a specific (normalised) velocity, where
  // kCoeffSlow for < 50m/s and kCoeffFast for > 50m/s because kCoeffFast is inaccurate
  // at < 10m/s but they both agree between ~10 and ~50m/s.
  static constexpr std::array<NavigationType, 16> kCoeffSlow = {
       136.3132, 158.9403,  63.6093, -35.4894, -149.2755, 152.6967, 502.5464, -218.4689,
      -779.534,   95.7285, 621.1013,  50.4598, -245.099,  -54.5,     38.0642,   12.3548};
  static constexpr std::array<NavigationType, 16> kCoeffFast = {
       258.6,  299.2,  115.2, -104.7, -260.9, 488.5, 940.8, -808.5, -1551.9,  551.7,
      1315.7,  -61.4, -551.4,  -84.5,   90.7,  26.2};

  NavigationType braking_distance = 2.0;
  NavigationType var = 1.0;
  if (getVelocity() < 50.0) {
    NavigationType norm_v = (getVelocity() - 30.0079) / 17.2325;
    for (unsigned int i = 0; i < kCoeffSlow.size(); ++i) {
      braking_distance += kCoeffSlow[i] * var;
      var *= norm_v;
    }
  } else {
    NavigationType norm_v = (getVelocity() - 41.4985) / 23.5436;
    for (unsigned int i = 0; i < kCoeffFast.size(); ++i) {
      braking_distance += kCoeffFast[i] * var;
      var *= norm_v;
    }
  }
  return braking_distance;
}

void Navigation::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  std::array<RollingStatistics<NavigationType>, data::Sensors::kNumImus> online_array =
    {RollingStatistics<NavigationType>(kCalibrationQueries),
     RollingStatistics<NavigationType>(kCalibrationQueries),
     RollingStatistics<NavigationType>(kCalibrationQueries),
     RollingStatistics<NavigationType>(kCalibrationQueries)};
  bool calibration_successful = false;
  int calibration_attempts = 0;

  while (!calibration_successful && calibration_attempts < kCalibrationAttempts) {
    log_.INFO("NAV", "Calibration attempt %d", calibration_attempts+1);

    // Average each sensor over specified number of readings
    for (int i = 0; i < kCalibrationQueries; ++i) {
      sensor_readings_ = data_.getSensorsImuData();
      for (int j = 0; j < data::Sensors::kNumImus; ++j) {
        online_array[j].update(sensor_readings_.value[j].acc[axis_]);
      }
      Thread::sleep(1);
    }
    // Check if each calibration's variance is acceptable
    calibration_successful = true;
    for (int i = 0; i < data::Sensors::kNumImus; ++i) {
      bool var_within_lim = online_array[i].getVariance() < calibration_limits_[axis_];
      calibration_successful = calibration_successful && var_within_lim;
    }
    calibration_attempts++;
  }

  // Store calibration and update filters if successful
  if (calibration_successful) {
    log_.INFO("NAV", "Calibration of IMU acceleration succeeded with final readings:");
    for (int i = 0; i < data::Sensors::kNumImus; ++i) {
      gravity_calibration_[i] = online_array[i].getMean();
      double var = online_array[i].getVariance();
      filters_[i].updateMeasurementCovarianceMatrix(var);

      log_.INFO("NAV", "\tIMU %d: g=%.5f, var=%.5f", i, gravity_calibration_[i], var);
    }
    status_ = ModuleStatus::kReady;
    updateData();
    log_.INFO("NAV", "Navigation module ready");
  } else {
    log_.INFO("NAV", "Calibration of IMU acceleration failed with final readings:");
    for (int i = 0; i < data::Sensors::kNumImus; ++i) {
      double acc = online_array[i].getMean();
      double var = online_array[i].getVariance();

      log_.INFO("NAV", "\tIMU %d: g=%.5f, var=%.5f", i, acc, var);
    }
    status_ = ModuleStatus::kCriticalFailure;
    updateData();
    log_.INFO("NAV", "Navigation module failed on calibration");
  }
}

void Navigation::queryImus()
{
  NavigationArray acc_raw;
  OnlineStatistics<NavigationType> acc_avg_filter;
  sensor_readings_ = data_.getSensorsImuData();
  uint32_t t = sensor_readings_.timestamp;
  // process raw values
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    acc_raw[i] = sensor_readings_.value[i].acc[axis_] - gravity_calibration_[i];
  }
  tukeyFences(acc_raw, kTukeyThreshold);
  // Kalman filter the readings
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    NavigationType estimate = filters_[i].filter(acc_raw[i]);
    acc_avg_filter.update(estimate);
  }
  acceleration_.value = acc_avg_filter.getMean();
  acceleration_.timestamp = t;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}

// TODO(Neil) - update to method suitable in general (assumes 4 IMUs)
void Navigation::tukeyFences(NavigationArray& data_array, float threshold)
{
  // copy the original array for sorting
  NavigationArray data_array_copy = data_array;
  // find the quartiles
  std::sort(data_array_copy.begin(), data_array_copy.end());
  float q1 = (data_array_copy[0]+data_array_copy[1]) / 2.;
  float q2 = (data_array_copy[1]+data_array_copy[2]) / 2.;
  float q3 = (data_array_copy[2]+data_array_copy[3]) / 2.;
  float iqr = q3 - q1;
  // find the thresholds
  float upper_limit = q3 + threshold*iqr;
  float lower_limit = q1 - threshold*iqr;
  // replace any outliers with the median
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    if (data_array[i] < lower_limit or data_array[i] > upper_limit) {
      log_.INFO("NAV", "Outlier detected in IMU %d, reading: %.3f. Updated to %.3f", //NOLINT
                       i, data_array[i], q2);
      data_array[i] = q2;
    }
  }
}

void Navigation::updateData()
{
  data::Navigation nav_data;
  nav_data.module_status              = getModuleStatus();
  nav_data.distance                   = getDistance();
  nav_data.velocity                   = getVelocity();
  nav_data.acceleration               = getAcceleration();
  nav_data.emergency_braking_distance = getEmergencyBrakingDistance();
  nav_data.braking_distance           = getBrakingDistance();

  data_.setNavigationData(nav_data);

  if (counter_ % kPrintFreq == 0) {
    log_.INFO("NAV", "%d: Data Update: a=%.3f, v=%.3f, d=%.3f", //NOLINT
                     counter_, nav_data.acceleration, nav_data.velocity, nav_data.distance);
  }
  counter_++;
}

void Navigation::navigate()
{
  queryImus();
  updateData();
}

void Navigation::initTimestamps()
{
  // First iteration --> set timestamps
  acceleration_.timestamp = utils::Timer::getTimeMicros();
  velocity_    .timestamp = utils::Timer::getTimeMicros();
  distance_    .timestamp = utils::Timer::getTimeMicros();
}

}}  // namespace hyped::navigation
