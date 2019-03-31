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

#ifndef PROPULSION_CALCULATE_RPM_HPP_
#define PROPULSION_CALCULATE_RPM_HPP_

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <cstdint>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

// #include "motor_control/communicator.hpp"
#include "propulsion/state_processor_interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "utils/concurrent/barrier.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"

namespace hyped
{

using data::NavigationType;
using utils::System;
using utils::Logger;
using utils::Timer;
using utils::concurrent::Barrier;
using utils::concurrent::Thread;

namespace motor_control
{

class CalculateRPM
{
    public:
        explicit CalculateRPM(Logger &log);

        bool initialize(std::string filepath);

    private:
        /**
     *   @brief  { Reads slip and translational velocity data from acceleration text files, calculates RPM's for appropriate slip at each
     *             translational velocity and stores the values in a 2D array containing
     *             translational velocity and RPM }
     */

    bool check_file(std::string filepath);

    std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> data);

    void RPM_calc(std::string filepath);

    int32_t calculateRPM(float velocity);

    data::Data &data_;
    data::StateMachine state_;
    data::Motors motor_data_;
    MotorVelocity motor_velocity_;

    std::queue<std::vector<double>> acceleration_slip_;

    Logger &log_;
};

}  // namespace motor_control
}  // namespace hyped

#endif  // PROPULSION_CALCULATE_RPM_HPP_
