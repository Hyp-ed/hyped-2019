/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for single IMU measurement filtered with Kalman Filter and written to file
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

#ifndef NAVIGATION_SINGLE_IMU_KALMAN_HPP_
#define NAVIGATION_SINGLE_IMU_KALMAN_HPP_

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/math/integrator.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"
#include "navigation/gravity_calibrator.hpp"
#include "navigation/imu_data_logger.hpp"
#include "navigation/imu_query.hpp"
#include "navigation/kalman_manager.hpp"

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <stdio.h>
#include <fstream>


namespace hyped
{
    using data::DataPoint;
    using data::ImuData;
    using data::NavigationVector;
    using data::NavigationEstimate;
    using sensors::Imu;
    using utils::Logger;
    using utils::math::Integrator;
    using utils::System;
    using utils::Timer;
    using navigation::GravityCalibrator;
    using navigation::ImuDataLogger;
    using navigation::ImuQuery;
    using navigation::KalmanManager;

    namespace navigation
    {
        class SingleImuKalmanNavigation
        {
            public:
                SingleImuKalmanNavigation();
                SingleImuKalmanNavigation(ImuQuery& imuQuery_, int imuId_, GravityCalibrator& gravityCalibrator,
                                          Timer* timer_, unsigned int n_, unsigned int m_);
                SingleImuKalmanNavigation(ImuQuery& imuQuery_, int imuId_, GravityCalibrator& gravityCalibrator,
                                          Timer* timer_, unsigned int n_, unsigned int m_, unsigned int k_);
                int navigate(unsigned int nTestQueries, float queryDelay, int runId, Logger log);

            private:
                ImuQuery&           imuQUery;
                GravityCalibrator&  gravityCalibrator;
                int                 imuId;
                Timer*              timer;
                KalmanManager       kalmanManager;
        };
    }
}

#endif  // NAVIGATION_SINGLE_IMU_KALMAN_HPP_ 
