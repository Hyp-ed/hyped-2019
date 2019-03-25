/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 2/03/2019
 * Description: Combine readings from multiple IMUs to determine acceleration, velocity and position
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

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/math/integrator.hpp"
#include "utils/math/statistics.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"

#include <iostream>
#include <fstream>

using hyped::data::DataPoint;
using hyped::data::ImuData;
using hyped::data::NavigationVector;
using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::math::Integrator;
using hyped::utils::math::OnlineStatistics;
using hyped::utils::math::RollingStatistics;
using hyped::utils::System;
using hyped::utils::Timer;

void outfileSetup(std::ofstream* outfile, int kRunImus, int run_id)  {
	char fname [40];
	sprintf(fname, "test_data/char_%dimu_run%d_data.csv", kRunImus, run_id);
	outfile->open(fname);
	*outfile << "arx0,ary0,arz0,arx1,ary1,arz1,arx2,ary2,arz2," 
			 << "arx3,ary3,arz3,arx4,ary4,arz4,arx5,ary5,arz5,"
			 << "arx6,ary6,arz6,t\n";
}

void querySensorArray(Imu** sensorArray, ImuData* sensorDataArray, 
					  DataPoint<NavigationVector*>* sensorReading, 
					  Timer* timer, int kRunImus) 
{
	// Take simple average of the IMUs
	for (int i = 0; i < kRunImus; ++i) 
	{
		sensorArray[i]->getData(sensorDataArray + i);
		sensorReading->value[i] = sensorDataArray[i].acc;		
	}
	// Update sensor reading
	sensorReading->timestamp = timer->getTimeMicros(); 
}

void writeToFile(std::ofstream* outfile, int kRunImus,
				 DataPoint<NavigationVector*>* sensorReading)
{
	for (int i = 0; i < kRunImus; ++i)
	{
		*outfile << sensorReading->value[i][0] << "," 
				 << sensorReading->value[i][1] << "," 
				 << sensorReading->value[i][2] << ",";
	}
	*outfile << sensorReading->timestamp << "\n";
}

int main(int argc, char *argv[])
{
	/******************************************
	 *		     	System Setup              *
	 ******************************************/
	// System setup
	System::parseArgs(argc, argv);
	System& sys = System::getSystem();
	Logger log(sys.verbose, sys.debug);
	Timer timer;

	log.INFO("MAIN", "Initialising...");

	// File setup
	std::ofstream outfile;
	bool saveReadings = sys.run_id > 0;
	if (saveReadings) outfileSetup(&outfile, sys.imu_count, sys.run_id);

	// Sensor setup
	int kNumImus = 7;
	int kRunImus = sys.imu_count;	
	Imu* sensorArray [kNumImus] = {new Imu(log,  49, 0x08, 0x00), 
								   new Imu(log, 117, 0x08, 0x00), 
								   new Imu(log, 115, 0x08, 0x00),
								   new Imu(log,  66, 0x08, 0x00),
								   new Imu(log,  67, 0x08, 0x00),
								   new Imu(log,  68, 0x08, 0x00),
								   new Imu(log,  69, 0x08, 0x00)};
	ImuData sensorDataArray [kNumImus] = {ImuData(), ImuData(), ImuData(), ImuData(),
										  ImuData(), ImuData(), ImuData()};

  	NavigationVector navigationArray [kNumImus] = {NavigationVector({0.,0.,0.}),
												   NavigationVector({0.,0.,0.}),
												   NavigationVector({0.,0.,0.}),
												   NavigationVector({0.,0.,0.}),
												   NavigationVector({0.,0.,0.}),
												   NavigationVector({0.,0.,0.}),
												   NavigationVector({0.,0.,0.})};
	DataPoint<NavigationVector*> sensorReading(0., navigationArray);

	// Test-run setup parameters
	int kNumTestQueries = 10000;

	/******************************************
	 *		     THE MAIN EVENT               *
	 ******************************************/	

	log.INFO("MAIN", "Measuring...");
	// Perform measurements
	for (int i = 0; i < kNumTestQueries; ++i)
	{
		// Query sensor
		querySensorArray(sensorArray, sensorDataArray, &sensorReading, 
					  	 &timer, kRunImus);

		// Output values
		if (saveReadings) writeToFile(&outfile, kRunImus, &sensorReading);
	}

	// Clean up
	if (saveReadings) outfile.close();


	return 0;
}