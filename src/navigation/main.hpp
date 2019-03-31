/*
 * Author:
 * Organisation: HYPED
 * Date:
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
#ifndef NAVIGATION_MAIN_HPP_
#define NAVIGATION_MAIN_HPP_

#include "data/data.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"
#include "sensors/imu.hpp"
#include "navigation/gravity_calibrator.hpp"
#include "navigation/imu_data_logger.hpp"
#include "navigation/imu_query.hpp"
#include "navigation/single_imu_navigation.hpp"

namespace hyped
{
    using data::ImuData;
    using utils::Logger;
    using utils::System;
    using utils::Timer;
    using sensors::Imu;

    using navigation::GravityCalibrator;
    using navigation::ImuDataLogger;
    using navigation::ImuQuery;
    using navigation::SingleImuNavigation;

    namespace navigation
    {
        int main(int argc, char *argv[]);
    }
}

#endif  // NAVIGATION_MAIN_HPP_
