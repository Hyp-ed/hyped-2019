/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 2/03/2019
 * Description: Simulate trajectory measurements using generated accelerations
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

#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <stdio.h>
#include <fstream>
#include <queue>
#include <random>
#include <string>

using hyped::data::DataPoint;
using hyped::data::ImuData;
using hyped::data::NavigationVector;
using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::math::Integrator;
using hyped::utils::math::RollingStatistics;
using hyped::utils::System;
using hyped::utils::Timer;

using std::default_random_engine;
using std::normal_distribution;
using std::ifstream;
using std::ofstream;
using std::queue;
using std::string;
using std::vector;
using std::copy;

typedef vector<NavigationVector> NavigationArray;

void loadSimData(queue<DataPoint<NavigationArray>>* dataQueue, int numImus,
				 queue<int>* stripeCount,
				 ifstream* accData, ifstream* posData,
				 string accFname, string posFname, 
				 float refreshRate, float stddev, float stripeSep) 
{
	float t = 0.;
	float ax, nx, ny, nz;
	// Open files
	accData->open(accFname);
	posData->open(posFname);
	// Simulate Gaussian noise
	default_random_engine generator;
	normal_distribution<float> noise(0., stddev);
	// Add noise to each acceleration reading and store
	while (*accData >> ax) {
		NavigationArray imus;
		// Add noise independently to each virtual imu
		for (int i = 0; i < numImus; ++i)
		{
			nx = noise(generator);
			ny = noise(generator);
			nz = noise(generator);
			imus.push_back(NavigationVector({ax+nx, ny, nz}));
		}
		// Convert time to microseconds
		dataQueue->push(DataPoint<NavigationArray>(t*1e6, imus));
		// Increment time
		t += refreshRate;
	}
	// Prepare stripe counts
	float dx;
	while(*posData >> dx) {
		stripeCount->push(static_cast<int>(dx/stripeSep));
	}
	// Close files
	accData->close();
	posData->close();
}

void getSensorAverage(NavigationArray imus, int numImus, 
		  			  RollingStatistics<NavigationVector>* rolling) 
{
	for (int i = 0; i < numImus; ++i) rolling->update(imus[i]);
}

void outfileSetup(ofstream* outfile, int run_id) 
{
	char fname [20];
	sprintf(fname, "sim_data/run%d_data.csv", run_id);
	outfile->open(fname);
	*outfile << "ax,ay,az,vx,vy,vz,sx,sy,sz,t\n";
}

void printToFile(ofstream* outfile, DataPoint<NavigationVector>* acc, 
									DataPoint<NavigationVector>* vel, 
									DataPoint<NavigationVector>* pos)
{
	*outfile << acc->value[0] << "," << acc->value[1] << "," << acc->value[2] << ","
	 		 << vel->value[0] << "," << vel->value[1] << "," << vel->value[2] << ","
			 << pos->value[0] << "," << pos->value[1] << "," << pos->value[2] << ","
			 << acc->timestamp << "\n";
}

int main(int argc, char *argv[])
{
	/*
		System setup
	*/
	System::parseArgs(argc, argv);
	System& sys = System::getSystem();
	Logger log(sys.verbose, sys.debug);
	bool writeToFile = sys.run_id > 0;

	/* 
		Sim data setup
	*/
	// sim parameters
	float refreshRate = 1./3000.;	// query frequency
	float stddev = 0.5;				// noise = 2*sensorVariance (empirical)
	float stripeSep = 100./3.281;	// stipe separation (100ft)
	int kNumImus = (sys.imu_count > 0) ? sys.imu_count : 8; 
	// file properties
	ifstream accData, posData;
	string accFname = "sim_data/acceleration-3k.txt";
	string posFname = "sim_data/displacement-3k.txt";
	// for quick access in sim
	queue<DataPoint<NavigationArray>> dataQueue;
	queue<int> stripeCount;
	// the main event
	loadSimData(&dataQueue, kNumImus, 
				&stripeCount,
				&accData, &posData, 
				accFname, posFname,
				refreshRate, stddev, stripeSep);
	/*
		Output setup
	*/
	ofstream outfile;
	if (writeToFile) outfileSetup(&outfile, sys.run_id);

	/*
		Simulate run
	*/
	// Store measured/estimated values;
	DataPoint<NavigationVector> acc(0., NavigationVector({0.,0.,0.})); 	
	DataPoint<NavigationVector> vel(0., NavigationVector({0.,0.,0.}));		
	DataPoint<NavigationVector> pos(0., NavigationVector({0.,0.,0.}));
	// Integrate acceleration -> velocity -> position
	Integrator<NavigationVector> velIntegrator(&vel);
	Integrator<NavigationVector> posIntegrator(&pos);
	// Determine mean and variance
	RollingStatistics<NavigationVector> rolling(kNumImus);
	// Check stipe count
	int stripesSeen = 0;
	// Perform acceleration, speed and distance queries
	while (!dataQueue.empty())
	{
		// ``Query'' sensors and filter
		getSensorAverage(dataQueue.front().value, kNumImus, &rolling);
		acc.value     = rolling.getMean();
		acc.timestamp = dataQueue.front().timestamp;
		dataQueue.pop();

		// Intergrate
		velIntegrator.update(acc);
		posIntegrator.update(vel);

		// Check if stripe has been passed
		if (stripeCount.front() != stripesSeen) {
			std::cout << "updating stripe count..." << std::endl;
			// Update position if we pass a stripe
			stripesSeen = stripeCount.front();
			pos.value = NavigationVector({stripesSeen*stripeSep, 0., 0.});
		}
		stripeCount.pop();

		// Output values
		log.INFO("MAIN", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f  s:%d\n", 
	  					acc.value[0], acc.value[1], acc.value[2], 
						vel.value[0], vel.value[1], vel.value[2], 
						pos.value[0], pos.value[1], pos.value[2],
						stripesSeen);

		if (writeToFile > 0) 
		{
			printToFile(&outfile, &acc, &vel, &pos);
		}
	}

	if (writeToFile) outfile.close();

	return 0;
}