/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 22/02/2019
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
using hyped::utils::math::OnlineStatistics;
using hyped::utils::System;
using hyped::utils::Timer;

using std::default_random_engine;
using std::normal_distribution;
using std::ifstream;
using std::ofstream;
using std::queue;
using std::string;

void loadSimData(queue<DataPoint<NavigationVector>>* dataQueue, ifstream* dataFile, float refreshRate, float sig) 
{
	float t = 0.;
	float ax, ay, az;
	// Simulate Gaussian noise
	default_random_engine generator;
	normal_distribution<float> noise(0., sig);
	// Add noise to each reading and store
	while (*dataFile >> ax) {
		ax += noise(generator);
		ay = noise(generator);
		az = noise(generator);
		dataQueue->push(DataPoint<NavigationVector>(t*1e6, NavigationVector({ax, ay, az})));
		// Increment time
		t += refreshRate;
	}
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
	// System setup
	System::parseArgs(argc, argv);
	System& sys = System::getSystem();
	Logger log(sys.verbose, sys.debug);
	bool writeToFile = sys.run_id > 0;

	// Test values
	float refreshRate = 1./3000.;
	float sig = 0.01;

	// Sim data setup
	ifstream dataFile;
	string fname = "sim_data/acceleration-3k.txt";
	dataFile.open(fname);
	queue<DataPoint<NavigationVector>> dataQueue;
	loadSimData(&dataQueue, &dataFile, refreshRate, sig);

	// Output setup
	ofstream outfile;
	if (writeToFile) outfileSetup(&outfile, sys.run_id);

	// Store measured/estimated values
	DataPoint<NavigationVector> acc(0., NavigationVector({0.,0.,0.})); 	
	DataPoint<NavigationVector> vel(0., NavigationVector({0.,0.,0.}));		
	DataPoint<NavigationVector> pos(0., NavigationVector({0.,0.,0.}));
	// Integrate acceleration -> velocity -> position
	Integrator<NavigationVector> velIntegrator(&vel);
	Integrator<NavigationVector> posIntegrator(&pos);

	// Perform acceleration, speed and distance queries
	while (!dataQueue.empty())
	{
		// ``Query'' sensor
		acc = dataQueue.front();
		dataQueue.pop();

		// Intergrate
		velIntegrator.update(acc);
		posIntegrator.update(vel);

		// Output values
		log.INFO("MAIN", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
	  					acc.value[0], acc.value[1], acc.value[2], 
						vel.value[0], vel.value[1], vel.value[2], 
						pos.value[0], pos.value[1], pos.value[2]);

		if (writeToFile > 0) 
		{
			printToFile(&outfile, &acc, &vel, &pos);
		}
	}

	if (writeToFile) outfile.close();

	return 0;
}