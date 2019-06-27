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
           calibration_limits_({0.05, 0.05, 0.05}),
           imu_reliable {true, true, true, true},
           nOutlierImus(0),
           stripe_counter_(0, 0),
           keyence_used(true),
           keyence_failure_counter_(0),
           acceleration_(0, 0.),
           velocity_(0, 0.),
           distance_(0, 0.),
           distance_uncertainty(0.),
           velocity_uncertainty(0.),
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
    // If the IMU is unreliable, set its reading to zero.
    if (!imu_reliable[i]) acc_raw[i] = 0;
  }
  tukeyFences(acc_raw, kTukeyThreshold);
  // Kalman filter the readings which are reliable
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    if (imu_reliable[i]) {
      NavigationType estimate = filters_[i].filter(acc_raw[i]);
      acc_avg_filter.update(estimate);
    }
  }
  acceleration_.value = acc_avg_filter.getMean();
  acceleration_.timestamp = t;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}

void Navigation::updateUncertainty()
{
  /* Uncertainty from measuring is the timestamp between two measurements times velocity.
   * Furthermore, the velocity has an uncertainty due to acceleration and timestamp. */
  double delta_t = (distance_.timestamp - prev_timestamp)/1000000.0;
  NavigationType abs_delta_acc = abs(getAcceleration() - prev_acc);
  // Adds uncertainty from the possible shift in both directions in the timestamp
  velocity_uncertainty += abs_delta_acc*delta_t/2;
  // Adds uncertainty from the variance in acceleration from measurements
  NavigationType acc_variance = 0.0;
  for (int i = 0; i < data::Sensors::kNumImus; i++) {
    acc_variance += filters_[i].KalmanFilter::getEstimateVariance();
  }
  // Average variance
  acc_variance = acc_variance/data::Sensors::kNumImus;
  // Standard deviation
  NavigationType acc_stdDev = sqrt(acc_variance);
  // uncertainty is the std deviation integrated to give velocity
  velocity_uncertainty += acc_stdDev*delta_t;
  // Hence uncertainty in distance becomes updated with:
  distance_uncertainty += velocity_uncertainty*delta_t;
}

void Navigation::queryKeyence()
{
  // initialise the keyence readings with the data from the central data struct
  keyence_readings_ = data_.getSensorsKeyenceData();
  for (int i = 0; i < data::Sensors::kNumKeyence; i++) {
    // Checks whether the stripe count has been updated and if it has not been
    // double-counted with the time constraint (100000micros atm, aka 0.1s, subject to change).
    if (prev_keyence_readings_[i].count.value != keyence_readings_[i].count.value &&
         keyence_readings_[i].count.timestamp - stripe_counter_.timestamp > 1e5) {
      stripe_counter_.value++;
      stripe_counter_.timestamp = keyence_readings_[i].count.timestamp;

      // Allow up to one missed stripe.
      // There must be some uncertainty in distance around the missed 30.48m.
      double allowed_uncertainty = distance_uncertainty;  // Temporary value
      NavigationType distance_change = distance_.value - stripe_counter_.value*30.48;
      if (distance_change > 30.48 - allowed_uncertainty &&
          distance_change < 30.48 + allowed_uncertainty &&
          distance_.value > stripe_counter_.value*30.48 + 0.5*30.48) {
        stripe_counter_.value++;
        distance_change -= 30.48;
      }
      /* Error handling: If distance from keyence still deviates more than the allowed
      uncertainty, then the measurements are no longer believable. Important that this
      is only checked in an update, otherwise we might throw an error in between stripes.
      The first stripe is very uncertain, since it takes the longest, thus we ignore it.
      Even if the first stripe is missed, error handling will catch it when the second is seen.*/
      if ((distance_change < -allowed_uncertainty  ||
           distance_change >  allowed_uncertainty) &&
           stripe_counter_.value > 1) {
        // TODO(Justus) what happens in case of keyence failure?
        keyence_failure_counter_++;
      }
      // Lower the uncertainty in velocity:
      velocity_uncertainty -= abs(distance_change*1e6/(2*
                              (stripe_counter_.timestamp - init_timestamp)));
      log_.INFO("NAV", "Timestamp difference: %d", stripe_counter_.timestamp - init_timestamp);
      log_.INFO("NAV", "Timestamp currently:  %d", stripe_counter_.timestamp);
      // Make sure velocity uncertainty is positive.
      velocity_uncertainty = abs(velocity_uncertainty);
      // The uncertainty in distance is not changed from this because the impact is far too large
      // Update velocity value
      velocity_.value -= distance_change*1e6/(stripe_counter_.timestamp - init_timestamp);
      // Update distance value
      distance_.value -= distance_change;
      break;
    }
  }
  prev_keyence_readings_ = keyence_readings_;
}

void Navigation::disableKeyenceUsage()
{
  keyence_used = false;
}

// TODO(Neil) - update to method suitable in general (assumes 4 IMUs)
void Navigation::tukeyFences(NavigationArray& data_array, float threshold)
{
  // copy the original array for sorting
  NavigationArray data_array_copy = data_array;
  // find the quartiles
  std::sort(data_array_copy.begin(), data_array_copy.end());
  // Define the quartiles first:
  float q1 = 0;
  float q2 = 0;
  float q3 = 0;
  // The most likely case is that all four IMUs are still reliable:
  if (nOutlierImus == 0) {
    q1 = (data_array_copy[0]+data_array_copy[1]) / 2.;
    q2 = (data_array_copy[1]+data_array_copy[2]) / 2.;
    q3 = (data_array_copy[2]+data_array_copy[3]) / 2.;
  // The second case is that one IMU is faulty
  } else if (nOutlierImus == 1) {
    // When copying we know there must be one zero value
    // There is definitely a more efficient way to write this... todo?
    if (!imu_reliable[0]) {
      q1 = (data_array_copy[1] + data_array_copy[2]) / 2.;
      q2 =  data_array_copy[2];
      q3 = (data_array_copy[2] + data_array_copy[3]) / 2.;
    } else if (!imu_reliable[1]) {
      q1 = (data_array_copy[0] + data_array_copy[2]) / 2.;
      q2 =  data_array_copy[2];
      q3 = (data_array_copy[2] + data_array_copy[3]) / 2.;
    } else if (!imu_reliable[2]) {
      q1 = (data_array_copy[0] + data_array_copy[1]) / 2.;
      q2 =  data_array_copy[1];
      q3 = (data_array_copy[1] + data_array_copy[3]) / 2.;
    } else if (!imu_reliable[3]) {
      q1 = (data_array_copy[0] + data_array_copy[1]) / 2.;
      q2 =  data_array_copy[1];
      q3 = (data_array_copy[1] + data_array_copy[2]) / 2.;
    }
  }
  // find the thresholds
  float iqr = q3 - q1;
  float upper_limit = q3 + threshold*iqr;
  float lower_limit = q1 - threshold*iqr;
  // replace any outliers with the median
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    if ((data_array[i] < lower_limit or data_array[i] > upper_limit) && imu_reliable[i]) {
      // log_.INFO("NAV", "Outlier detected in IMU %d, reading: %.3f. Updated to %.3f", //NOLINT
      //                 i, data_array[i], q2);
      data_array[i] = q2;
      imu_outlier_counter_[i]++;
      // If this counter exceeds some threshold then that IMU is deemed unreliable
      if (imu_outlier_counter_[i] > 1000 && imu_reliable[i]) {
        imu_reliable[i] = false;
        nOutlierImus++;
      }
    } else {
      imu_outlier_counter_[i] = 0;
    }
  }
  if (counter_ % 1000 == 0) {
    log_.INFO("NAV", "Outliers: IMU1: %d, IMU2: %d, IMU3: %d, IMU4: %d", imu_outlier_counter_[0],
      imu_outlier_counter_[1], imu_outlier_counter_[2], imu_outlier_counter_[3]);
    log_.INFO("NAV", "Number of outliers: %d", nOutlierImus);
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

  if (counter_ % 1 == 0) {  // kPrintFreq
    // log_.INFO("NAV", "%d: Data Update: a=%.3f, v=%.3f, d=%.3f, d(gpio)=%.3f", //NOLINT
    //               counter_, nav_data.acceleration, nav_data.velocity, nav_data.distance,
    //               stripe_counter_.value*30.48);
    // log_.INFO("NAV", "%d: Data Update: v(unc)=%.3f, d(unc)=%.3f, keyence failures: %d",
    //            counter_, velocity_uncertainty, distance_uncertainty, keyence_failure_counter_);
  }
  counter_++;
  // Update all prev measurements
  prev_timestamp = distance_.timestamp;
  prev_acc = getAcceleration();
}

void Navigation::navigate()
{
  queryImus();
  if (keyence_used) queryKeyence();
  updateUncertainty();
  updateData();
}

void Navigation::initTimestamps()
{
  // First iteration --> set timestamps
  acceleration_.timestamp = utils::Timer::getTimeMicros();
  velocity_    .timestamp = utils::Timer::getTimeMicros();
  distance_    .timestamp = utils::Timer::getTimeMicros();
  init_timestamp = utils::Timer::getTimeMicros();
  log_.INFO("NAV", "Initial timestamp:%d", init_timestamp);
  prev_timestamp = utils::Timer::getTimeMicros();
  // First iteration --> get initial keyence data
  // (should be zero counters and corresponding timestamp)
  prev_keyence_readings_ = data_.getSensorsKeyenceData();
}
}}  // namespace hyped::navigation
