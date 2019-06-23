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
#include <iostream>

#include "navigation/navigation_new.hpp"

namespace hyped {

namespace navigation {

Navigation::Navigation(Logger& log, bool write_to_outfile)
         : log_(log),
           sys_(System::getSystem()),
           data_(Data::getInstance()),
           config_(*sys_.config),
           filter_(data::Sensors::kNumImus),
           integrator_acc_to_vel_(&vel_val_),
           integrator_vel_to_pos_(&pos_val_),
           write_to_outfile_(write_to_outfile),
           outfile_base_(config_.navigation.outfile_base),
           outfile_name_(config_.navigation.outfile_name)

{
  log_.INFO("NAV", "Navigation module started");
  if (write_to_outfile_) {
    prepareOutfile();
  }
}

Navigation::~Navigation()
{
  if (write_to_outfile_) {
    closeOutfile();
  }
}

void Navigation::navigate()
{
  queryImus();
  filterAcceleration();
  estimateTrajectory();
  writeToOutfile();
}

void Navigation::queryImus()
{
  imu_data_array_ = data_.getSensorsImuData();
  acc_raw_array_.timestamp = imu_data_array_.timestamp;
  acc_cor_array_.timestamp = imu_data_array_.timestamp;
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    acc_raw_array_.value[i] = imu_data_array_.value[i].acc;
    acc_cor_array_.value[i] = acc_raw_array_.value[i] - acc_offset_array_[i];
  }
}

void Navigation::filterAcceleration()
{
  acc_val_.timestamp = acc_cor_array_.timestamp;
  acc_var_.timestamp = acc_cor_array_.timestamp;
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    filter_.update(acc_cor_array_.value[i]);
  }
  acc_val_.value = filter_.getMean();
  acc_var_.value = filter_.getVariance();
}

void Navigation::estimateTrajectory()
{
  integrator_acc_to_vel_.update(acc_val_);
  integrator_vel_to_pos_.update(vel_val_);
}

void Navigation::determineCalibration()
{
  log_.INFO("NAV", "Calibrating");
  // Capped size means we disregard old readings if recalibrating
  RollingArray rolling_array;
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    rolling_array[i].setWindowSize(kCalibrationQueries);
  }
  for (int q = 0; q < kCalibrationQueries; ++q) {
    imu_data_array_ = data_.getSensorsImuData();
    for (int i = 0; i < data::Sensors::kNumImus; ++i) {
      rolling_array[i].update(imu_data_array_.value[i].acc);
    }
  }
  log_.INFO("NAV", "Determined calibration:");
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    acc_offset_array_[i] = rolling_array[i].getMean();
    log_.INFO("NAV", "\tIMU%d: x - %.3f,  y - %.3f,  z - %.3f",
              i, acc_offset_array_[i][0], acc_offset_array_[i][1], acc_offset_array_[i][2]);
  }
}

void Navigation::prepareOutfile()
{
  outfile_.open(outfile_path_.c_str());
  // Write header
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    outfile_ << "arx" << i << ",ary" << i << ",arz" << i << ",";
  }
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    outfile_ << "acx" << i << ",acy" << i << ",acz" << i << ",";
  }
  outfile_ << "amx,amy,amz,asx,asy,asz,vx,vy,vz,px,py,pz,t\n";
  log_.INFO("NAV", "Outfile ready");
}

void Navigation::writeToOutfile()
{
  writeNavigationArray(data::Sensors::kNumImus, acc_raw_array_.value);
  writeNavigationArray(data::Sensors::kNumImus, acc_cor_array_.value);
  writeNavigationVector(acc_val_.value);
  writeNavigationVector(acc_var_.value);
  writeNavigationVector(vel_val_.value);
  writeNavigationVector(pos_val_.value);
  outfile_ << acc_raw_array_.timestamp << "\n";
}

void Navigation::writeNavigationArray(uint8_t num_elements, NavigationArray navigation_array)
{
  for (int i = 0; i < num_elements; ++i) {
    writeNavigationVector(navigation_array[i]);
  }
}

void Navigation::writeNavigationVector(NavigationVector navigation_vector)
{
  for (int a = 0; a < 3; ++a) {
    outfile_ << navigation_vector[a] << ",";
  }
}

void Navigation::closeOutfile()
{
  outfile_.close();
}

}}  // namespace hyped::navigation
