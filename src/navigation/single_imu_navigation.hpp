/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for simple single IMU measurement written to file
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

#ifndef NAVIGATION_SINGLE_IMU_NAVIGATION_HPP_
#define NAVIGATION_SINGLE_IMU_NAVIGATION_HPP_

#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <cstdio>
#include <fstream>

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/math/integrator.hpp"
#include "utils/system.hpp"
#include "navigation/gravity_calibrator.hpp"
#include "navigation/imu_data_logger.hpp"
#include "navigation/imu_query.hpp"

namespace hyped
{
    using data::DataPoint;
    using data::ImuData;
    using data::NavigationVector;
    using sensors::Imu;
    using utils::Logger;
    using utils::math::Integrator;
    using utils::math::OnlineStatistics;
    using utils::System;
    using navigation::GravityCalibrator;
    using navigation::ImuDataLogger;
    using navigation::ImuQuery;

    namespace navigation
    {
        class SingleImuNavigation
        {
            public:
                SingleImuNavigation(ImuQuery& imuQuery_, int imuId_,
                                    GravityCalibrator& gravityCalibrator_);
                int navigate(unsigned int nTestQueries, float queryDelay, int runId, Logger& log);

            private:
                ImuQuery&           imuQuery;
                GravityCalibrator&  gravityCalibrator;
                int                 imuId;
        };
    }
}

#endif  // NAVIGATION_SINGLE_IMU_NAVIGATION_HPP_
