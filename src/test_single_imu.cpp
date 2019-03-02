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

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/math/integrator.hpp"
#include "utils/math/statistics.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <stdio.h>
#include <fstream>

using hyped::data::DataPoint;
using hyped::data::ImuData;
using hyped::data::NavigationVector;
using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::math::Integrator;
using hyped::utils::math::OnlineStatistics;
using hyped::utils::System;
using hyped::utils::Timer;

DataPoint<NavigationVector> queryImuAcceleration(Imu* imu, ImuData* imuData, Timer* timer) 
{
	imu->getData(imuData);
	DataPoint<NavigationVector> acc(timer->getTimeMicros(), imuData->acc);
	return acc;
}

NavigationVector calibrateGravity(Imu* imu, ImuData* imuData, unsigned int nCalQueries, Timer* timer) 
{
	OnlineStatistics<NavigationVector> online;
	for (unsigned int i = 0; i < nCalQueries; ++i)
	{
		online.update(queryImuAcceleration(imu, imuData, timer).value);
		sleep((float)1/nCalQueries);
	}
	return online.getMean();
}

void outfileSetup(std::ofstream* outfile, int imu_id, int run_id) 
{
	char fname [20];
	sprintf(fname, "test_data/imu%d_run%d_data.csv", imu_id, run_id);
	outfile->open(fname);
	*outfile << "arx,ary,arz,acx,acy,acz,vx,vy,vz,sx,sy,sz,t\n";
}

void printToFile(std::ofstream* outfile, DataPoint<NavigationVector>* accRaw, DataPoint<NavigationVector>* accCor,
										 DataPoint<NavigationVector>*    vel, DataPoint<NavigationVector>*    pos)
{
	*outfile << accRaw->value[0] << "," << accRaw->value[1] << "," << accRaw->value[2] << ","
	 		 << accCor->value[0] << "," << accCor->value[1] << "," << accCor->value[2] << ","
	 		 <<    vel->value[0] << "," <<    vel->value[1] << "," <<    vel->value[2] << ","
			 <<    pos->value[0] << "," <<    pos->value[1] << "," <<    pos->value[2] << ","
			 << accRaw->timestamp << "\n";
}

int main(int argc, char *argv[])
{
	// System setup
	System::parseArgs(argc, argv);
	System& sys = System::getSystem();
	Logger log(sys.verbose, sys.debug);
	Timer timer;
	bool writeToFile = (sys.imu_id > 0) || (sys.run_id > 0);

	// Sensor setup
	int i2c = 66;
	Imu* imu = new Imu(log, i2c, 0x08, 0x00);
	ImuData* imuData = new ImuData();

	// Test values
	unsigned int nCalQueries = 10000;
	unsigned int nTestQueries = 50000;
	float queryDelay = 0.01;

	// File setup
	std::ofstream outfile;
	if (writeToFile) outfileSetup(&outfile, sys.imu_id, sys.run_id);

	// Calibrate gravitational acceleration
	NavigationVector gVector = calibrateGravity(imu, imuData, nCalQueries, &timer);
//	std::cout << "Calibration complete, measuring." << std::endl;
	// Return measured gravity vector
	log.INFO("MAIN", "Measured gravity vector:\n\tgx=%+6.3f\tgy=%+6.3f\tgz=%+6.3f\n\n",
						gVector[0], gVector[1], gVector[2]);

	// Store measured/estimated values
	DataPoint<NavigationVector> accRaw(0., NavigationVector({0.,0.,0.}));
	DataPoint<NavigationVector> accCor(0., NavigationVector({0.,0.,0.})); 	
	DataPoint<NavigationVector>    vel(0., NavigationVector({0.,0.,0.}));		
	DataPoint<NavigationVector>    pos(0., NavigationVector({0.,0.,0.}));
	// Integrate acceleration -> velocity -> position
	Integrator<NavigationVector> velIntegrator(&vel);
	Integrator<NavigationVector> posIntegrator(&pos);

	// Perform acceleration, speed and distance measurements
	for (unsigned int i = 0; i < nTestQueries; ++i)
	{
		// Query sensor and correct values
		accRaw = queryImuAcceleration(imu, imuData, &timer);
		accCor = DataPoint<NavigationVector>(accRaw.timestamp, accRaw.value - gVector);

		// Intergrate
		velIntegrator.update(accCor);
		posIntegrator.update(vel);

		// Output values
		log.INFO("MAIN", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
	  					accCor.value[0], accCor.value[1], accCor.value[2], 
	  							 vel.value[0], vel.value[1], vel.value[2], 
	  							 pos.value[0], pos.value[1], pos.value[2]);		
		if (writeToFile > 0) 
		{
			printToFile(&outfile, &accRaw, &accCor, &vel, &pos);
		}


		sleep(queryDelay);
	}

	if (writeToFile) outfile.close();

	return 0;
}
