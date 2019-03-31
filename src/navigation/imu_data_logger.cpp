/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Imu data logger to write IMU data to CSV files
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

#include "imu_data_logger.hpp"

namespace hyped
{
    namespace navigation
    {
        ImuDataLogger::ImuDataLogger(std::ofstream* outfile_)
            : outfile(outfile_)
        {}

        void ImuDataLogger::setup(int imu_id, int run_id)
        {
            char fname[20];
            snprintf(fname, "test_data/imu%d_run%d_data.csv", imu_id, run_id);
            outfile->open(fname);
            *outfile << "arx,ary,arz,acx,acy,acz,vx,vy,vz,sx,sy,sz,t\n";
        }

        void ImuDataLogger::setupKalman(int imu_id, int run_id)
        {
            char fname[20];
            snprintf(fname, "test_data/imu%d_run%d_data.csv", imu_id, run_id);
            outfile->open(fname);
            *outfile << "arx,ary,arz,acx,acy,acz,vx,vy,vz,sx,sy,sz,sfx,sfy,sfz,vfx,vfy,vfz,"
                        "afx,afy,afz,t\n";
        }

        void ImuDataLogger::dataToFile(DataPoint<NavigationVector>* accR,
                                       DataPoint<NavigationVector>* accC,
                                       DataPoint<NavigationVector>*  vel,
                                       DataPoint<NavigationVector>*  pos)
        {
            *outfile << accR->value[0] << "," << accR->value[1] << "," << accR->value[2] << ","
                     << accC->value[0] << "," << accC->value[1] << "," << accC->value[2] << ","
                     <<  vel->value[0] << "," <<  vel->value[1] << "," <<  vel->value[2] << ","
                     <<  pos->value[0] << "," <<  pos->value[1] << "," <<  pos->value[2] << ","
                     << accR->timestamp << "\n";
        }

        void ImuDataLogger::dataToFileKalman(DataPoint<NavigationVector>* accR,
                                             DataPoint<NavigationVector>* accC,
                                             DataPoint<NavigationVector>*    vel,
                                             DataPoint<NavigationVector>*    pos,
                                             NavigationEstimate& x)
        {
            *outfile << accR->value[0] << "," << accR->value[1] << "," << accR->value[2] << ","
                     << accC->value[0] << "," << accC->value[1] << "," << accC->value[2] << ","
                     <<  vel->value[0] << "," <<  vel->value[1] << "," <<  vel->value[2] << ","
                     <<  pos->value[0] << "," <<  pos->value[1] << "," <<  pos->value[2] << ","
                     <<  x[0][0]       << "," <<  x[0][1]       << "," <<  x[0][2]       << ","
                     <<  x[1][0]       << "," <<  x[1][1]       << "," <<  x[1][2]       << ","
                     <<  x[2][0]       << "," <<  x[2][1]       << "," <<  x[2][2]       << ","
                     << accR->timestamp << "\n";
        }
    }
}
