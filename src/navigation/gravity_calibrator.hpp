/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for gravity calibration system
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

#ifndef NAVIGATION_GRAVITY_CALIBRATOR_HPP_
#define NAVIGATION_GRAVITY_CALIBRATOR_HPP_

#include <thread>
#include <chrono>

#include "data/data_point.hpp"
#include "utils/math/statistics.hpp"
#include "navigation/imu_query.hpp"

using std::this_thread::sleep_for;
using std::chrono::seconds;

namespace hyped
{
    using data::DataPoint;
    using data::NavigationVector;
    using utils::math::OnlineStatistics;
    using navigation::ImuQuery;

    namespace navigation
    {
        class GravityCalibrator
        {
            public:
                explicit GravityCalibrator(unsigned int numberCalibrationQueries_);
                NavigationVector calibrate(ImuQuery& imuQuery);
            private:
                unsigned int                        numberCalibrationQueries;
                unsigned int                        queryDelay;
                OnlineStatistics<NavigationVector>  online;
        };
    }
}

#endif  // NAVIGATION_GRAVITY_CALIBRATOR_HPP_
