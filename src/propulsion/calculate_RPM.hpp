/*
 * Author: George Karabassis
 * Organisation: HYPED
 * Date: 30/3/2018
 * Description:
 *
 *    Copyright 2018 HYPED
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

#ifndef BEAGLEBONE_BLACK_MOTOR_CONTROL_MAIN_HPP_
#define BEAGLEBONE_BLACK_MOTOR_CONTROL_MAIN_HPP_

#include <cstdint>
#include <vector>
#include <queue>
#include <string>

// #include "motor_control/communicator.hpp"
#include "propulsion/state_processor_interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/concurrent/barrier.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"

namespace hyped {

using data::NavigationType;
using utils::concurrent::Thread;
using utils::concurrent::Barrier;
using utils::Logger;
using utils::Timer;

namespace motor_control {

class CalculateRPM{

    public:

        CalculateRPM(Logger& log);

    private:
  /**
   *   @brief  { Reads slip and translational velocity data from acceleration and
   *             deceleration text files, calculates RPM's for appropriate slip at each
   *             translational velocity and stores the values in a 2D array containing
   *             translational velocity and RPM }
   */

        bool check_file(std::string filepath);

        std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> data);

        void RPM_calc(std::string filepath);

        int32_t calculateRPM(float velocity);

        data::Data& data_;
        data::StateMachine state_;
        data::Motors motor_data_;
        MotorVelocity motor_velocity_;

        std::queue<std::vector<double>> acceleration_slip_;

        Logger& log_;

        bool motor_failure_;
        bool slip_calculated_;

        float velocity;
        // int32_t slip_velocity;
};

}}  // namespace hyped::motor_control

#endif  // BEAGLEBONE_BLACK_MOTOR_CONTROL_MAIN_HPP_



