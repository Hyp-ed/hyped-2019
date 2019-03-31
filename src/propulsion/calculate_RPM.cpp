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

#include "propulsion/calculate_RPM.hpp"

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <cstdint>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

// #include "motor_control/communicator.hpp"
#include "data/data.hpp"
#include "utils/system.hpp"

namespace hyped
{

using data::NavigationType;
using utils::System;
using utils::Timer;

namespace motor_control
{

constexpr double kHalbachRadius = 0.148;
const std::string kAccelerationData = "../BeagleBone_black/data/configuration/AccelerationSlip.txt";

CalculateRPM::CalculateRPM(Logger &log) : data_(data::Data::getInstance()),
                                          acceleration_slip_(),
                                          log_(log)
{
    state_ = data_.getStateMachineData();
    motor_velocity_ = {0, 0, 0, 0};
}

std::vector<std::vector<double>> CalculateRPM::transpose(std::vector<std::vector<double>> data)
{
    std::vector<std::vector<double>> transpose(2, std::vector<double>(data.size(), 1));
    for (uint16_t i = 0; i < data.size(); ++i) {
        for (uint16_t j = 0; j < 2; ++j) {
            transpose[j][i] = data[i][j];
        }
    }
    return transpose;
}

bool CalculateRPM::check_file(std::string filepath)
{
    std::ifstream data;
    data.open(filepath);

    if (!data.is_open()) {
        log_.ERR("MOTOR", "Could not open file: %s", filepath.c_str());
        return false;
    } else if (filepath == kAccelerationData) {
        log_.INFO("MOTOR", "Calculating acceleration slip...");
    } else {
        log_.INFO("MOTOR", "Calculating deceleration slip...");
    }

    return true;
};

void CalculateRPM::RPM_calc(std::string filepath)
{
    double slip, translational_velocity, angular_velocity, rpm;
    std::ifstream data;
    data.open(filepath);

    std::string line;
    while (std::getline(data, line)) {
        std::string split_line;
        std::vector<double> temp_vec;
        std::stringstream ss(line);
        while (std::getline(ss, split_line, '\t')) {
            temp_vec.push_back(std::move(stod(split_line)));
        }

        // Calculate angular velocity from slip, translational velocity and halbach wheel radius
        slip = temp_vec[0];
        translational_velocity = temp_vec[1];
        angular_velocity = (slip + translational_velocity) / kHalbachRadius;

        // Calculate RPM given calculated angular velocity
        rpm = (angular_velocity * 60) / (2 * M_PI);

        // Use temporary vector to hold translational velocity and rpm
        temp_vec[0] = translational_velocity;
        temp_vec[1] = rpm;

        // Add data to the acceleration_slip_ queue
        acceleration_slip_.push(temp_vec);
    }

    // // If both containers have been populated, set bool to true
    // if (!acceleration_slip_.empty() && !deceleration_slip_.empty()) {
    //     acceleration_slip_ = transpose(acceleration_slip_);
    //     deceleration_slip_ = transpose(deceleration_slip_);
    //     slip_calculated_ = true;
    //     log_.INFO("MOTOR", "All slip values calculated");
    // }
};

int32_t CalculateRPM::calculateRPM(float velocity)
{
    do {
        if (velocity < acceleration_slip_.front()[0]) {
            return acceleration_slip_.front()[1];
        }
        if (acceleration_slip_.size() == 1) {
            return acceleration_slip_.front()[1];
        }
        acceleration_slip_.pop();
    } while (acceleration_slip_.front()[0] < velocity);

    return 0;
}

}  // namespace motor_control
}  // namespace hyped
