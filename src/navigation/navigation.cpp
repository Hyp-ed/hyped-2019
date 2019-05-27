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

#include "navigation/navigation.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/timer.hpp"

namespace hyped {

using hyped::utils::concurrent::Thread;

namespace navigation {

Navigation::Navigation(Logger& log, unsigned int axis/*=0*/)
         : log_(log),
           data_(Data::getInstance()),
           counter_(0),
           axis_(axis),
           filter_(1, 1),
           acceleration_(0., 0.),
           velocity_(0., 0.),
           distance_(0., 0.),
           acceleration_integrator_(&velocity_),
           velocity_integrator_(&distance_)
{
  filter_.setup();
  calibrateGravity();

  acceleration_.timestamp = utils::Timer::getTimeMicros();
  velocity_.timestamp = utils::Timer::getTimeMicros();
  distance_.timestamp = utils::Timer::getTimeMicros();
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

// TODO(Neil): add delay between samples?
void Navigation::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  std::array<OnlineStatistics<NavigationType>, data::Sensors::kNumImus> online_array;
  // Average each sensor over specified number of readings
  for (int i = 0; i < kNumCalibrationQueries; ++i) {
    sensor_readings_ = data_.getSensorsImuData();
    for (int j = 0; j < data::Sensors::kNumImus; ++j) {
      online_array[j].update(sensor_readings_.value[j].acc[axis_]);
    }
    Thread::sleep(1);
  }
  OnlineStatistics<NavigationType> var_statistics;
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    gravity_calibration_[i] = online_array[i].getMean();
    double var = online_array[i].getVariance();
    var_statistics.update(var);
    log_.INFO("NAV",
      "Update: g=%.5f, var=%.5f", gravity_calibration_[i], var);
  }
  filter_.updateMeasurementCovarianceMatrix(var_statistics.getMean());
}

void Navigation::queryImus()
{
  OnlineStatistics<NavigationType> avg_filter;
  sensor_readings_ = data_.getSensorsImuData();
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    // Apply calibrated correction
    avg_filter.update(sensor_readings_.value[i].acc[axis_] - gravity_calibration_[i]);
  }
  uint32_t t = sensor_readings_.timestamp;
  // passed time in second
  double dt = (t - acceleration_.timestamp)/1e6;
  filter_.updateStateTransitionMatrix(dt);
  measurement_ = avg_filter.getMean();
  NavigationType estimate = filter_.filter(measurement_);

  acceleration_.value = estimate;
  acceleration_.timestamp = t;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}

void Navigation::updateData()
{
  // Take new readings first
  queryImus();
  counter_ += 1;

  data::Navigation nav_data;
  nav_data.distance                   = getDistance();
  nav_data.velocity                   = getVelocity();
  nav_data.acceleration               = getAcceleration();
  nav_data.emergency_braking_distance = getEmergencyBrakingDistance();
  nav_data.braking_distance           = getBrakingDistance();

  data_.setNavigationData(nav_data);

  // Crude test of data writing
  nav_data = data_.getNavigationData();

  if (counter_ % 1 == 0) {
      log_.INFO("NAV",
          "%d: Update: a=%.3f, z=%.3f, v=%.3f, d=%.3f", //NOLINT
          counter_, nav_data.acceleration, measurement_, nav_data.velocity, nav_data.distance);
      // NavigationType var = filter_.getEstimateVariance();
      // log_.INFO("NAV", "Estimate acc variance: %.5f", var);
  }
}

}}  // namespace hyped::navigation
