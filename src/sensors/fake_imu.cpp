/*
 * Author:
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class for fake IMUs
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

#include <math.h>
#include <random>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/fake_imu.hpp"
#include "utils/timer.hpp"
#include "utils/math/statistics.hpp"


namespace hyped {
using utils::math::OnlineStatistics;
namespace sensors {

FakeImuFromFile::FakeImuFromFile(utils::Logger& log,
                std::string acc_file_path,
                std::string dec_file_path,
                std::string em_file_path)
    : log_(log),
      acc_noise_(1),
      acc_file_path_(acc_file_path),
      dec_file_path_(dec_file_path),
      em_file_path_(em_file_path),
      acc_started_(false),
      dec_started_(false),
      em_started_(false),
      data_(data::Data::getInstance())
{
  NavigationVector acc;
  acc[0] = 0.0;
  acc[1] = 0.0;
  acc[2] = 9.8;
  acc_val_ = acc;
  readDataFromFile(acc_file_path_, dec_file_path_, em_file_path_);
  log_.INFO("Fake-IMU", "Fake IMU initialised");
}


void FakeImuFromFile::startAcc()
{
  imu_ref_time_ = utils::Timer::getTimeMicros();
  acc_count_ = 0;
}

void FakeImuFromFile::startDec()
{
  imu_ref_time_ = utils::Timer::getTimeMicros();
  acc_count_ = 0;
}

void FakeImuFromFile::startEm()
{
  imu_ref_time_ = utils::Timer::getTimeMicros();
  acc_count_ =  0;
}

void FakeImuFromFile::getData(ImuData* imu)
{
  data::State state = data_.getStateMachineData().current_state;
  bool operational = true;
  if (state == data::State::kAccelerating) {
    // start acc
    if (!acc_started_) {
      acc_started_ = true;
      startAcc();
    }

    if (accCheckTime()) {
      acc_count_ = std::min(acc_count_, (int64_t) acc_val_read_.size());
      // Check so you don't go out of bounds
      if (acc_count_ == (int64_t) acc_val_read_.size()) {
        prev_acc_ = acc_val_read_[acc_count_- 1];
        operational = acc_val_operational_[acc_count_ - 1];
      } else {
        prev_acc_ = acc_val_read_[acc_count_];
        operational = acc_val_operational_[acc_count_];
      }
    }

  } else if (state == data::State::kNominalBraking) {
    if (!dec_started_) {
      log_.INFO("Fake-IMUs", "Start decelerating...");
      dec_started_ = true;
      startDec();
    }

    if (accCheckTime()) {
      acc_count_ = std::min(acc_count_, (int64_t) dec_val_read_.size());
      // Check so you don't go out of bounds
      if (acc_count_ == (int64_t) dec_val_read_.size()) {
        prev_acc_ = dec_val_read_[acc_count_-1];
        operational = dec_val_operational_[acc_count_-1];
      } else {
        prev_acc_ = dec_val_read_[acc_count_];
        operational = dec_val_operational_[acc_count_];
      }
    }

  } else if (state == data::State::kEmergencyBraking) {
    if (!em_started_) {
      log_.INFO("Fake-IMUs", "Start emergency breaking...");
      em_started_ = true;
      startEm();
    }

    if (accCheckTime()) {
      acc_count_ = std::min(acc_count_, (int64_t) em_val_read_.size());
      // Check so you don't go out of bounds
      if (acc_count_ == (int64_t) em_val_read_.size()) {
        prev_acc_ = em_val_read_[acc_count_-1];
      } else {
        prev_acc_ = em_val_read_[acc_count_];
      }
      operational = true;
    }

  } else {
    prev_acc_ = addNoiseToData(acc_val_, acc_noise_);
    operational = true;
  }
  imu->acc = prev_acc_;
  imu->operational = operational;
}

NavigationVector FakeImuFromFile::addNoiseToData(NavigationVector value, NavigationVector noise)
{
  NavigationVector temp;
  static std::default_random_engine generator;

  for (int i = 0; i < 3; i++) {
    std::normal_distribution<NavigationType> distribution(value[i], noise[i]);
    temp[i] = distribution(generator);
  }
  return temp;
}

void FakeImuFromFile::readDataFromFile(std::string acc_file_path,
                               std::string dec_file_path,
                               std::string em_file_path)
{
  for (int i = 0; i < 3; i++) {
    std::string file_path;
    uint32_t timestamp;
    std::vector<NavigationVector>* val_read;
    std::vector<bool>* bool_read;

    if (i == 0) {
      file_path = acc_file_path;
      timestamp = kAccTimeInterval;
      val_read  = &acc_val_read_;
      bool_read = &acc_val_operational_;
    } else if (i == 1) {
      file_path = dec_file_path;
      timestamp = kAccTimeInterval;
      val_read  = &dec_val_read_;
      bool_read = &dec_val_operational_;
    } else if (i == 2) {
      file_path = em_file_path;
      timestamp = kAccTimeInterval;
      val_read  = &em_val_read_;
      bool_read = &em_val_operational_;
    }
    std::ifstream file;
    file.open(file_path);
    if (!file.is_open()) {
      log_.ERR("Fake-IMU", "Wrong file path for argument: %d", i);
    }

    NavigationVector value, noise;
    double temp_value[10];
    int counter = 0;
    uint32_t temp_time;
    std::string line;

    while (getline(file, line)) {
      std::stringstream input(line);
      input >> temp_time;

      if (temp_time != timestamp*counter) {
        log_.ERR("Fake-IMU", "Timestamp format invalid %d", temp_time);
      }

      int value_counter = 0;
      while (input >> temp_value[value_counter] && value_counter < 10) {
        value_counter++;
      }

      if (value_counter != 7) {
        log_.ERR("Fake-IMU", "Incomplete values for the argument timestamp: %d", temp_time);
      }

      for (int i = 0; i < 3; i++)
        value[i] = temp_value[i];
      for (int i = 0; i < 3; i++)
        noise[i] = temp_value[i+3];

      val_read->push_back(addNoiseToData(value, noise));
      bool_read->push_back(temp_value[6]);

      counter++;
    }

    file.close();
  }
}

bool FakeImuFromFile::accCheckTime()
{
  uint64_t now = utils::Timer::getTimeMicros();
  uint64_t time_span = (now - imu_ref_time_) / 1000;

  if (time_span < kAccTimeInterval*acc_count_) {
    return false;
  }
  acc_count_ = time_span/kAccTimeInterval + 1;
  return true;
}

// FakeAccurateImu::FakeAccurateImu(utils::Logger& log)
//     : data_(data::Data::getInstance()),
//       acc_noise_(1),
//       log_(log)
// { /* EMPTY */ }

// void FakeAccurateImu::getData(ImuData* imu)
// {
//   data::Navigation nav = data_.getNavigationData();
//   data::Motors     mot = data_.getMotorData();
//   data::StateMachine      stm = data_.getStateMachineData();

//   if (stm.current_state == data::State::kEmergencyBraking) {
//     imu->acc[0] = -25;
//   } else {
//     // get average rmp
//     double rpm = 0;
//     rpm += mot.velocity_1;
//     rpm += mot.velocity_2;
//     rpm += mot.velocity_3;
//     rpm += mot.velocity_4;
//     rpm /= 4;

//     // get angular velocity
//     double velocity = (rpm*2*3.14159265358979323846*0.148)/60;
//     uint32_t scale = 4;
//     if (!std::isnan(nav.velocity))
//       imu->acc[0] = (velocity - nav.velocity)/scale;
//     else
//       imu->acc[0] = 0.0;
//   }
//   imu->acc[1] = 0;
//   imu->acc[2] = 9.8;

//   imu->acc = FakeImuFromFile::addNoiseToData(imu->acc, acc_noise_);
//   imu->operational = true;
// }

}}  // namespace hyped::sensors
