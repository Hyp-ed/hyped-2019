#include "utils/math/statistics.hpp"

using hyped::data::ImuData;
using hyped::data::NavigationVector;
using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::math::OnlineStatistics


/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 02/02/2019
 * Description: Simple single IMU measurement written to file
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
NavigationVector calibrateGravity(Imu& imu, ImuData& imuData, unsigned int nCalQueries) {
	OnlineStatistics<NavigationVector> online;
	for (int i = 0; i < nCalQueries; ++i)
	{
		imu->getData(imuData);
		online.update(imu->acc);
		sleep((float)1/nCalQueries);
	}
	return online.getMean();
}

int main(int argc, char const *argv[])
{
	// System setup
	hyped::utils::System::parseArgs(argc, argv);
	Logger& log = hyped::utils::System::getLogger();
	System& sys = hyped::utils::System::getSystem();

	// Sensor setup
	int i2c = 66;
	Imu * imu = new Imu(log, i2c, 0x08, 0x00);
	ImuData * imuData = new ImuData();

	// Test values
	unsigned int nQueries = 5000;
	unsigned int nCalQueries = 1000;

	// Calibrate gravitational acceleration
	NavigationVector gVector = calibrateGravity(imu, imuData, nCalQueries);
	
	// Return measured gravity vector
	std::cout << "Measured gravity vector:\n\tgx=" << gVector[0] << " gy=" << gVector[1] << " gz=" << gVector[2] << std::endl;

	return 0;
}