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
           status_(ModuleStatus::kStart),
           counter_(0),
           axis_(axis),
           stripe_counter_(0, 0),
           acceleration_(0, 0.),
           velocity_(0, 0.),
           distance_(0, 0.),
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
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    gravity_calibration_[i] = online_array[i].getMean();
    double var = online_array[i].getVariance();
    log_.INFO("NAV",
      "Update: g=%.5f, var=%.5f", gravity_calibration_[i], var);
    filters_[i].updateMeasurementCovarianceMatrix(var);
  }
  // After calibration is complete we are ready to measure
  status_ = ModuleStatus::kReady;
  updateData();
  log_.INFO("NAV", "Navigation module ready");
}

void Navigation::queryImus()
{
  OnlineStatistics<NavigationType> acc_avg_filter;
  sensor_readings_ = data_.getSensorsImuData();
  uint32_t t = sensor_readings_.timestamp;
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    // Apply calibrated correction
    NavigationType acc = sensor_readings_.value[i].acc[axis_] - gravity_calibration_[i];
    NavigationType estimate = filters_[i].filter(acc);
    acc_avg_filter.update(estimate);
  }
  acceleration_.value = acc_avg_filter.getMean();
  acceleration_.timestamp = t;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}
double Navigation::estimateDistanceUncertainty()
{
  /* The function to get the uncertainty simply based on the uncertainty in time and
  acceleration is obtained from s = 1/2*at², such that due to partial derivatives, the
  uncertainty is given by: */
  /* a is the current acceleration, t the current time, dadt the derivative of a with respect to
  t at the current time, deltaA is the uncertainty in acceleration at the current time. The derivation
  of this can be uploaded at request. */
  double deltaA = 0.0;  // Temporary value
  double dadt = 0.0;    // Temporary value
  double a = acceleration_.value;
  double t = (utils::Timer::getTimeMicros())/100000.0;
  double uncertainty = sqrt((4*a*a*t*t + 2*a*t*t*t*(dadt) + t*t*t*t*(dadt)*(dadt)/4)*0.000001 +
                                  t*t*t*t*deltaA*deltaA/4);
  // TODO(Justus) add practical uncertainty to this.
  return uncertainty;
}
void Navigation::queryKeyence()
{
  // initialise the keyence readings with the data from the central data struct
  keyence_readings_ = data_.getSensorsKeyenceData();
  for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
    // Checks whether the stripe count has been updated and if it has not been
    // double-counted with the time constraint (100000micros atm, aka 0.1s, subject to change).
    if (prev_keyence_readings_[i].count.value != keyence_readings_[i].count.value &&
         keyence_readings_[i].count.timestamp -
         prev_keyence_readings_[i].count.timestamp > 100000) {
      stripe_counter_.value++;
      stripe_counter_.timestamp = keyence_readings_[i].count.timestamp;

      // Allow up to one missed stripe.
      // There must be some uncertainty in distance around the missed 30.48m.
      double allowed_uncertainty = 10.0;  // Temporary value, estimateDistanceUncertainty()
      if (distance_.value - stripe_counter_.value*30.48 > 30.48 - allowed_uncertainty &&
          distance_.value - stripe_counter_.value*30.48 < 30.48 + allowed_uncertainty) {
        stripe_counter_.value++;
      }
      /* Error handling: If distance from keyence still deviates more than the allowed
      uncertainty, then the measurements are no longer believable. Important that this
      is only checked in an update, otherwise we might throw an error in between stripes. */
      if (distance_.value - stripe_counter_.value*30.48 > 0.0 - allowed_uncertainty &&
          distance_.value - stripe_counter_.value*30.48 < allowed_uncertainty) {
        // TODO(Justus) what happens in case of keyence failure?
      }
      velocity_.value -= (distance_.value - stripe_counter_.value*30.48)
        *1000000/stripe_counter_.timestamp;
      distance_.value = stripe_counter_.value*30.48;
      break;
    }
  }
  prev_keyence_readings_ = keyence_readings_;
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

  if (counter_ % 1000 == 0) {  // kPrintFreq
    log_.INFO("NAV", "%d: Data Update: a=%.3f, v=%.3f, d=%.3f, d(keyence)=%.3f", //NOLINT
                     counter_, nav_data.acceleration, nav_data.velocity, nav_data.distance,
                     stripe_counter_.value*30.48);
  }
  counter_++;
}

void Navigation::navigate()
{
  queryImus();
  queryKeyence();
  updateData();
}

void Navigation::initTimestamps()
{
  // First iteration --> set timestamps
  acceleration_.timestamp = utils::Timer::getTimeMicros();
  velocity_    .timestamp = utils::Timer::getTimeMicros();
  distance_    .timestamp = utils::Timer::getTimeMicros();
  // First iteration --> get initial keyence data
  // (should be zero counters and corresponding timestamp)
  prev_keyence_readings_ = data_.getSensorsKeyenceData();
}

}}  // namespace hyped::navigation
