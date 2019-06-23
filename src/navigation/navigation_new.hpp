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
#ifndef NAVIGATION_NAVIGATION_NEW_HPP_
#define NAVIGATION_NAVIGATION_NEW_HPP_

#include <array>
#include <fstream>
#include <string>

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "utils/config.hpp"
#include "utils/logger.hpp"
#include "utils/math/statistics.hpp"
#include "utils/math/integrator.hpp"
#include "utils/system.hpp"


namespace hyped {

using data::Data;
using data::DataPoint;
using data::ImuData;
using data::NavigationVector;
using utils::Config;
using utils::Logger;
using utils::math::Integrator;
using utils::math::RollingStatistics;
using utils::System;


namespace navigation {

  class Navigation {
    public:
      Navigation(Logger& log, bool write_to_outfile);
      ~Navigation();
      /**
       * @brief Query sensors, determine acceleration, velocity, position etc - the
       * meat'n'potatoes of nav.
       */
      void navigate();
      /**
       * @brief Store average of kCalibrationQueries values of IMU acceleration, taken
       * at rest, to be subracted from future measurements. Accounts for gravity and
       * constant sensor offset.
       */
      void determineCalibration();

    private:
      typedef std::array<ImuData, data::Sensors::kNumImus> ImuDataArray;
      typedef std::array<NavigationVector, data::Sensors::kNumImus> NavigationArray;
      typedef std::array<RollingStatistics<NavigationVector>, data::Sensors::kNumImus> RollingArray; // NOLINT

      // System communication
      Logger& log_;
      System& sys_;
      Data& data_;
      const Config& config_;

      // Store estimated values
      DataPoint<ImuDataArray> imu_data_array_;
      DataPoint<NavigationArray> acc_raw_array_;
      DataPoint<NavigationArray> acc_cor_array_;
      DataPoint<NavigationVector> acc_val_;
      DataPoint<NavigationVector> acc_var_;
      DataPoint<NavigationVector> vel_val_;
      DataPoint<NavigationVector> pos_val_;

      // Filter multiple IMU readings into single value
      RollingStatistics<NavigationVector> filter_;

      // Convert acceleration -> velocity -> position
      Integrator<NavigationVector> integrator_acc_to_vel_;
      Integrator<NavigationVector> integrator_vel_to_pos_;

      // Calibration
      static constexpr int kCalibrationQueries = 10000;
      NavigationArray acc_offset_array_;

      // Data recording
      bool write_to_outfile_;
      std::string outfile_base_;  // defined in config.txt
      std::string outfile_name_;  // defined in config.txt
      std::string outfile_path_ = outfile_base_ + outfile_name_;
      std::ofstream outfile_;

      /**
       * @brief Query sensors to update stored acceleration.
       */
      void queryImus();
      /**
       * @brief Combine all IMU readings into a single estimate.
       */
      void filterAcceleration();
      /**
       * @brief Filter acceleration then integrate to get veclotiy and position.
       */
      void estimateTrajectory();
      /**
       * @brief Open outfile and write header in preparation for logging.
       */
      void prepareOutfile();
      /**
       * @brief Write to outfile to log collected nav information.
       */
      void writeToOutfile();
      /**
       * @brief Write a NavigationArray neatly in .csv format
       */
      void writeNavigationArray(uint8_t num_elements, NavigationArray navigation_array);
      /**
       * @brief Write a NavigationVector neatly in .csv format
       */
      void writeNavigationVector(NavigationVector navigation_vector);
      /**
       * @brief Close outfile gracefully.
       */
      void closeOutfile();
};
}}  // namespace hyped::navigation

#endif  // NAVIGATION_NAVIGATION_NEW_HPP_
