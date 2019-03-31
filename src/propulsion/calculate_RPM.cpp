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

#include <string>
#include <vector>

namespace hyped
{

namespace motor_control
{

constexpr double kHalbachRadius = 0.148;
const std::string kAccelerationData = "../BeagleBone_black/data/configuration/AccelerationSlip.txt";

CalculateRPM::CalculateRPM(Logger &log) : data_(data::Data::getInstance()),
                                          acceleration_slip_(),
                                          log_(log)
{
}

bool CalculateRPM::initialize(std::string filepath)
{
    if (check_file(filepath)) {
        RPM_calc(filepath);
        return true;
    }
    return false;
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
        if (acceleration_slip_.empty()) {
            return 6000;
        }
        acceleration_slip_.pop();
    } while (acceleration_slip_.front()[0] < velocity);

    return 6000;
}

}  // namespace motor_control
}  // namespace hyped