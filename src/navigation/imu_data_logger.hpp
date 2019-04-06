/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for Imu data logger to write IMU data to CSV files
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

#ifndef NAVIGATION_IMU_DATA_LOGGER_HPP_
#define NAVIGATION_IMU_DATA_LOGGER_HPP_

#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <cstdio>
#include <fstream>

#include "data/data.hpp"
#include "data/data_point.hpp"

namespace hyped {
using data::NavigationVector;
using data::NavigationEstimate;
using data::DataPoint;

namespace navigation {

class ImuDataLogger
{
  public:
    explicit ImuDataLogger(std::ofstream* outfile_);
    void setup(int imu_id, int run_id);
    void setupKalman(int imu_id, int run_id);
    void dataToFileSimulation(DataPoint<NavigationVector>* acc,
                  DataPoint<NavigationVector>* vel,
                  DataPoint<NavigationVector>* pos);
    void dataToFile(DataPoint<NavigationVector>* accRaw,
            DataPoint<NavigationVector>* accCor,
            DataPoint<NavigationVector>*  vel,
            DataPoint<NavigationVector>*  pos);
    void dataToFileKalman(DataPoint<NavigationVector>* accRaw,
                DataPoint<NavigationVector>* accCor,
                DataPoint<NavigationVector>*  vel,
                DataPoint<NavigationVector>*  pos,
                NavigationEstimate& x);
  private:
    std::ofstream*   outfile;
};
}}  // namespace hyped navigation

#endif  // NAVIGATION_IMU_DATA_LOGGER_HPP_
