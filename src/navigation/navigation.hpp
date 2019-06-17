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

#ifndef NAVIGATION_NAVIGATION_HPP_
#define NAVIGATION_NAVIGATION_HPP_

#include <array>
#include <cstdint>
#include <math.h>

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/imu.hpp"
#include "navigation/kalman_filter.hpp"
#include "utils/logger.hpp"
#include "utils/math/integrator.hpp"
#include "utils/math/statistics.hpp"

namespace hyped {

using data::Data;
using data::DataPoint;
using data::ImuData;
using data::ModuleStatus;
using data::NavigationType;
using data::NavigationVector;
using navigation::KalmanFilter;
using utils::Logger;
using utils::math::Integrator;
using utils::math::OnlineStatistics;

namespace navigation {

  class Navigation {
    public:
      typedef std::array<ImuData, data::Sensors::kNumImus> ImuDataArray;
      typedef DataPoint<ImuDataArray>  ImuDataPointArray;
      typedef std::array<NavigationType, data::Sensors::kNumImus> NavigationArray;
      typedef std::array<KalmanFilter, data::Sensors::kNumImus> FilterArray;
      typedef array<data::StripeCounter, data::Sensors::kNumKeyence> KeyenceDataArray;

      /**
       * @brief Construct a new Navigation object
       *
       * @param log System logger
       * @param axis Axis used of acceleration measurements
       */
      explicit Navigation(Logger& log, unsigned int axis = 0);
      /**
       * @brief Get the current state of the navigation module
       *
       * @return ModuleStatus the current state of the navigation module
       */
      ModuleStatus getModuleStatus() const;
      /**
       * @brief Get the measured acceleration [m/s^2]
       *
       * @return NavigationType Returns the forward component of acceleration vector (negative when
       *                        decelerating) [m/s^2]
       */
      NavigationType getAcceleration() const;
      /**
       * @brief Get the measured velocity [m/s]
       *
       * @return NavigationType Returns the forward component of velocity vector [m/s]
       */
      NavigationType getVelocity() const;
      /**
       * @brief Get the measured displacement [m]
       *
       * @return NavigationType Returns the forward component of displacement vector [m]
       */
      NavigationType getDistance() const;
      /**
       * @brief Get the emergency braking distance [m]
       *
       * @return NavigationType emergency braking distance [m]
       */
      NavigationType getEmergencyBrakingDistance() const;
      /**
       * @brief Get the braking distance [m]
       *
       * @return NavigationType braking distance [m]
       */
      NavigationType getBrakingDistance() const;
      /**
       * @brief Get the determined gravity calibration [m/s^2]
       *
       * @return NavitationArray recorded gravitational acceleration [m/s^2]
       */
      NavigationArray getGravityCalibration() const;
      /**
       * @brief Determine the value of gravitational acceleration measured by sensors at rest
       */
      void calibrateGravity();
      /**
       * @brief Update central data structure
       */
      void updateData();
      /**
       * @brief Take acceleration readings from IMU, filter, integrate and then update central data
       * structure with new values (i.e. the meat'n'potatoes of navigation).
       */
      void navigate();
      /**
       * @brief Initialise timestamps for integration
       */
      void initTimestamps();
      /**
       * @brief Disable keyence readings to have any impact on the run.
       */
      void disableKeyenceUsage();

    private:
      static constexpr int kNumCalibrationQueries = 10000;
      static constexpr int kPrintFreq = 1;
      static constexpr NavigationType kEmergencyDeceleration = 24;

      // System communication
      Logger& log_;
      Data& data_;
      ModuleStatus status_;

      // counter for outputs
      unsigned int counter_;

      // movement axis
      unsigned int axis_;


      // Kalman filters to filter each IMU measurement individually
      FilterArray filters_;

      // Stripe counter (rolling values)
      DataPoint<uint32_t> stripe_counter_;
      // Keyence data read
      KeyenceDataArray keyence_readings_;
      // Previous keyence data for comparison
      KeyenceDataArray prev_keyence_readings_;
      // Are the keyence sensors used or ignored?
      bool keyence_used;


      // To store estimated values
      ImuDataPointArray sensor_readings_;
      DataPoint<NavigationType> acceleration_;
      DataPoint<NavigationType> velocity_;
      DataPoint<NavigationType> distance_;
      NavigationArray gravity_calibration_;

      // Previous timestamp
      uint32_t prev_timestamp;
      // Uncertainty in distance
      NavigationType distance_uncertainty;
      // Uncertainty in velocity
      NavigationType velocity_uncertainty;
      /* Previous two acceleration measurements, necessary for uncertainty determination
       * The [0]th index stands for the previous measurement, [1] to the one before that.
       */
      std::array<NavigationType, 2> prev_accs;

      // To convert acceleration -> velocity -> distance
      Integrator<NavigationType> acceleration_integrator_;  // acceleration to velocity
      Integrator<NavigationType> velocity_integrator_;      // velocity to distance

      /**
       * @brief Query sensors to determine acceleration, velocity and distance
       */
      void queryImus();
      /**
       * @brief Query Keyence sensors to determine whether a stripe is found, update stripe_counter_ accordingly
       */
      void queryKeyence();
      /**
       * @brief Update uncertainty in distance obtained through IMU measurements.
       */
      void updateUncertainty();
  };


}}  // namespace hyped::navigation

#endif  // NAVIGATION_NAVIGATION_HPP_
