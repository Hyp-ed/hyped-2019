/*
 * Organisation: HYPED
 * Date: 
 * Description: Class for data exchange between sub-team threads and structures
 * for holding data produced by each of the sub-teams.
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

#ifndef BEAGLEBONE_BLACK_DATA_DATA_HPP_
#define BEAGLEBONE_BLACK_DATA_DATA_HPP_

#include "utils/math/vector.hpp"

namespace hyped {

// imports
using utils::math::Vector;

namespace data {

// -------------------------------------------------------------------------------------------------
// Navigation
// -------------------------------------------------------------------------------------------------
typedef float NavigationType;
struct Navigation {
  NavigationType  distance;
  NavigationType  velocity;
  NavigationType  acceleration;
  NavigationType  emergency_braking_distance;
  NavigationType  braking_distance = 750;  // TODO(Brano): Remove default,publish the actual dist.
};

// -------------------------------------------------------------------------------------------------
// Raw Sensor data
// -------------------------------------------------------------------------------------------------
struct Sensor {
  bool operational;
};

typedef Vector<NavigationType, 3> NavigationVector;
struct ImuData : public Sensor {
  NavigationVector acc;
  NavigationVector gyr;
};

}}  // namespace hyped::data

#endif  // BEAGLEBONE_BLACK_DATA_DATA_HPP_