/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Gravity calibration system for determining gravity acceleration in navigation
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

#include "gravity_calibrator.hpp"

namespace hyped
{
    namespace navigation
    {
        GravityCalibrator::GravityCalibrator(unsigned int numberCalibrationQueries_)
            : numberCalibrationQueries(numberCalibrationQueries_),
              queryDelay(static_cast<float>(1)/numberCalibrationQueries)
        {
            OnlineStatistics<NavigationVector> online;
        }

        NavigationVector GravityCalibrator::calibrate(ImuQuery& imuQuery)
        {
            for (unsigned int i = 0; i < numberCalibrationQueries; ++i) {
                online.update(imuQuery.query().value);
                sleep_for(seconds(queryDelay));
            }
            return online.getMean();
        }
    }
}
