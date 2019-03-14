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

void outfileSetup(std::ofstream* outfile, int imu_id, int run_id)  {
	char fname [20];
	sprintf(fname, "test_data/imu%d_run%d_data.csv", imu_id, run_id);
	outfile->open(fname);
	*outfile << "arx,ary,arz,acx,acy,acz,vx,vy,vz,sx,sy,sz,t\n";
}

void querySensorArray(Imu** sensorArray, ImuData* sensorDataArray, 
					  RollingStatistics<NavigationVector>* filter, Timer* timer,
					  DataPoint<NavigationVector>* sensorReading, 
					  int kNumImus) 
{
	// Take simple average of the IMUs
	for (int i = 0; i < kNumImus; ++i) 
	{
		sensorArray[i]->getData(sensorDataArray + i);
		filter->update(sensorDataArray[i].acc);		// <-- TODO, don't think this will work as intended
	}
	// Update sensor reading
	sensorReading->timestamp = timer->getTimeMicros(); 
	sensorReading->value = filter->getMean();	
}

NavigationVector calibrateG(Imu** sensorArray, ImuData* sensorDataArray, 
 							RollingStatistics<NavigationVector>* filter, Timer* timer,
					  		DataPoint<NavigationVector>* sensorReading,
							int kNumCalQueries, int kNumImus) 
{
	OnlineStatistics<NavigationVector> queryAggregator;
	for (int i = 0; i < kNumCalQueries; ++i) 
	{
		querySensorArray(sensorArray, sensorDataArray, filter,
						 timer, sensorReading, kNumImus);
		queryAggregator.update(sensorReading->value);
	}
	return queryAggregator.getMean();
}

void writeToLog(Logger* log, 
				DataPoint<NavigationVector>* acc, DataPoint<NavigationVector>* vel, DataPoint<NavigationVector>* pos)
{
	log->INFO("MAIN", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
	  					acc->value[0], acc->value[1], acc->value[2], 
						vel->value[0], vel->value[1], vel->value[2], 
						pos->value[0], pos->value[1], pos->value[2]);					
}

void writeToFile(std::ofstream* outfile, 
				 DataPoint<NavigationVector>* accRaw, DataPoint<NavigationVector>* accCor,
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
	bool saveReadings = (sys.imu_id > 0) || (sys.run_id > 0);
	if (saveReadings) outfileSetup(&outfile, sys.imu_id, sys.run_id);

	// Sensor setup    TODO[NEIL], donde esta la imus?
	/*
	int kNumImus = 8;	
	Imu* sensorArray [kNumImus] = {new Imu(log, 66, 0x08, 0x00), new Imu(log, -1, 0x08, 0x00), 
								   new Imu(log, -1, 0x08, 0x00), new Imu(log, -1, 0x08, 0x00),
								   new Imu(log, -1, 0x08, 0x00), new Imu(log, -1, 0x08, 0x00),
								   new Imu(log, -1, 0x08, 0x00), new Imu(log, -1, 0x08, 0x00)};
	ImuData sensorDataArray [kNumImus] = {ImuData(), ImuData(), ImuData(), ImuData(),
										  ImuData(), ImuData(), ImuData(), ImuData()};
										  */
	int kNumImus = 7;	
	Imu* sensorArray [kNumImus] = {new Imu(log,  66, 0x08, 0x00), 
								   new Imu(log,  67, 0x08, 0x00), 
								   new Imu(log,  68, 0x08, 0x00),
								   new Imu(log,  20, 0x08, 0x00),
								   new Imu(log, 115, 0x08, 0x00),
								   new Imu(log, 117, 0x08, 0x00),
								   new Imu(log,  49, 0x08, 0x00)};
	ImuData sensorDataArray [kNumImus] = {ImuData(), ImuData(), ImuData(), ImuData(),
										  ImuData(), ImuData(), ImuData()};

  	DataPoint<NavigationVector> sensorReading;

  	// Filter setup
  	RollingStatistics<NavigationVector> filter(kNumImus);

	// Test-run setup parameters
	int kNumCalQueries = 10000;
	int kNumTestQueries = 10000;


	/******************************************
	 *		     	Calibration               *
	 ******************************************/
	log.INFO("MAIN", "Calibrating...");
	// Calibrate gravity
	NavigationVector gVector = calibrateG(sensorArray, sensorDataArray, 
										  &filter, &timer, 
										  &sensorReading, 
										  kNumCalQueries, kNumImus);
	log.INFO("MAIN", "Measured gravity vector:\n\tgx=%+6.3f\tgy=%+6.3f\tgz=%+6.3f\n\n",
						gVector[0], gVector[1], gVector[2]);
	// TODO: kalman filter calibration?

	/******************************************
	 *		     THE MAIN EVENT               *
	 ******************************************/
	// Store measured/estimated values
	DataPoint<NavigationVector> accRaw(0., NavigationVector({0.,0.,0.}));
	DataPoint<NavigationVector> accCor(0., NavigationVector({0.,0.,0.})); 	
	DataPoint<NavigationVector>    vel(0., NavigationVector({0.,0.,0.}));		
	DataPoint<NavigationVector>    pos(0., NavigationVector({0.,0.,0.}));
	// Integrate acceleration -> velocity -> position
	Integrator<NavigationVector> velIntegrator(&vel);
	Integrator<NavigationVector> posIntegrator(&pos);

	log.INFO("MAIN", "Measuring...");
	// Perform measurements
	for (int i = 0; i < kNumTestQueries; ++i)
	{
		// Query sensor
		querySensorArray(sensorArray, sensorDataArray, 
					  	 &filter, &timer,
					  	 &accRaw, 
					  	 kNumImus);
		// Correct values and integrate
		accCor.timestamp = accRaw.timestamp;	accCor.value = accRaw.value - gVector;
		velIntegrator.update(accCor);
		posIntegrator.update(vel);

		// Output values
		writeToLog(&log, &accCor, &vel, &pos);
		if (saveReadings) writeToFile(&outfile, &accRaw, &accCor, &vel, &pos);
	}

	// Clean up
	if (saveReadings) outfile.close();


	return 0;
}