/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: IMU query for navigation
 *
 *  Copyright 2019 HYPED
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *  except in compliance with the License. You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under
 *  the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "imu_query.hpp"

namespace hyped {
namespace navigation {
ImuQuery::ImuQuery(Imu* imu_, ImuData* imuData_, Timer* timer_)
  : imu(imu_),
    imuData(imuData_),
    timer(timer_)
{}

DataPoint<NavigationVector> ImuQuery::query()
{
  imu->getData(imuData);
  DataPoint<NavigationVector> acc(timer->getTimeMicros(), imuData->acc);
  return acc;
}
}}  // namespace hyped navigation
