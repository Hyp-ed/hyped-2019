/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for IMU query for navigation
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

#ifndef NAVIGATION_IMU_QUERY_HPP_
#define NAVIGATION_IMU_QUERY_HPP_

#include "data/data_point.hpp"
#include "data/data.hpp"
#include "sensors/imu.hpp"
#include "utils/timer.hpp"

namespace hyped
{
    using data::DataPoint;
    using data::ImuData;
    using data::NavigationVector;
    using sensors::Imu;
    using utils::Timer;

    namespace navigation
    {
        class ImuQuery
        {
            public:
                ImuQuery(Imu* imu_, ImuData* imuData_, Timer* timer_);
                DataPoint<NavigationVector> query();
            private:
                Imu*        imu;
                ImuData*    imuData;
                Timer*      timer;
        };
    }
}

#endif  // NAVIGATION_IMU_QUERY_HPP_
