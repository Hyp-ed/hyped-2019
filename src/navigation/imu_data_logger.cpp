/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Imu data logger to write IMU data to CSV files
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

#include "imu_data_logger.hpp"

namespace hyped {
namespace navigation {
ImuDataLogger::ImuDataLogger(std::ofstream* outfile_)
  : outfile(outfile_)
{}

void ImuDataLogger::setup(int imu_id, int run_id)
{
  char fname[100];
  snprintf(fname, sizeof(fname), "test_data/imu%d_run%d_data.csv", imu_id, run_id);
  outfile->open(fname);
  *outfile << "ar,ac,v,s,t\n";
}

void ImuDataLogger::setupKalman(int imu_id, int run_id)
{
  char fname[100];
  snprintf(fname, sizeof(fname), "test_data/imu%d_run%d_data.csv", imu_id, run_id);
  outfile->open(fname);
  *outfile << "ar,ac,v,s,sf,vf,af,t\n";
}

void ImuDataLogger::dataToFileSimulation(DataPoint<NavigationType>* acc,
                     DataPoint<NavigationType>* vel,
                     DataPoint<NavigationType>* pos)
{
  *outfile << acc->value << "," << vel->value << "," << pos->value << ","
           << acc->timestamp << "\n";
}

void ImuDataLogger::dataToFile(DataPoint<NavigationType>* accR,
                 DataPoint<NavigationType>* accC,
                 DataPoint<NavigationType>*  vel,
                 DataPoint<NavigationType>*  pos)
{
  *outfile << accR->value << "," << accC->value << "," << vel->value << "," <<  pos->value << ","
           << accR->timestamp << "\n";
}

void ImuDataLogger::dataToFileKalman(DataPoint<NavigationType>* accR,
                   DataPoint<NavigationType>* accC,
                   DataPoint<NavigationType>*  vel,
                   DataPoint<NavigationType>*  pos,
                   NavigationVector& x)
{
  *outfile << accR->value << "," << accC->value << "," <<  vel->value << "," <<  pos->value << ","
           <<  x[0]       << "," <<  x[1]       << "," <<  x[2]       << ","
           << accR->timestamp << "\n";
}
}}  // namespace hyped navigation
