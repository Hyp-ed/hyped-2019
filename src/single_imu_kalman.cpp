/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 10/03/2019
 * Description: single IMU measurement filtered with Kalman Filter and written to file
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
#include "utils/math/kalman_multivariate.hpp"
#include "utils/system.hpp"
#include "utils/timer.hpp"

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <stdio.h>
#include <fstream>
#include<cmath>
#include <random>

#include <Eigen/Dense>

using hyped::data::DataPoint;
using hyped::data::ImuData;
using hyped::data::NavigationVector;
using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::math::Integrator;
using hyped::utils::math::OnlineStatistics;
using hyped::utils::math::KalmanMvar;
using hyped::utils::System;
using hyped::utils::Timer;

const Eigen::MatrixXd createInitialErrorCovarianceMatrix(unsigned int n)
{
    Eigen::MatrixXd P = Eigen::MatrixXd::Constant(n, n, 0.0);
    std::default_random_engine generator;
    std::normal_distribution<double> pos_var_noise(0.0,0.001);
    std::normal_distribution<double> vel_var_noise(0.0,0.005);
    std::normal_distribution<double> acc_var_noise(0.0,0.01);

    for (unsigned int i = 0; i< n; i++)
    {
        if (i < n/3)
        {
            P(i, i) = pos_var_noise(generator);
        } else if (i < 2*n/3)
        {
            P(i, i) = vel_var_noise(generator);
        } else
        {
            P(i, i) = acc_var_noise(generator);
        }
    }
    return P;
}

const Eigen::MatrixXd createStateTransitionMatrix(unsigned int n, double dt)
{
    Eigen::MatrixXd A(n, n);
    double acc_ddt = 0.5 * dt*dt;
    A << 1.0, 0.0, 0.0, dt, 0.0, 0.0, acc_ddt, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0, dt, 0.0, 0.0, acc_ddt, 0.0,
         0.0, 0.0, 1.0, 0.0, 0.0, dt, 0.0, 0.0, acc_ddt,
         0.0, 0.0, 0.0, 1.0, 0.0, 0.0, dt, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, dt, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, dt,
         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0;
    return A;
}

const Eigen::MatrixXd createStateTransitionCovarianceMatrix(unsigned int n)
{
    std::default_random_engine generator;
    std::normal_distribution<double> var_noise(0.01,0.02);

    Eigen::MatrixXd Q = Eigen::MatrixXd::Constant(n, n, 0.0);
    /*
    for (unsigned int row = 0; row < n; row++)
    {
        for (unsigned int col = 0; col < n; col++)
        {
            Q(row, col) = var_noise(generator);
        }
    }
    */
    return Q;
}

const Eigen::MatrixXd createStationaryMeasurementCovarianceMatrix(unsigned int m)
{
    std::default_random_engine generator;
    std::normal_distribution<double> var_noise(0.0,0.0005);
    std::normal_distribution<double> cov_noise(0.0,0.0001);

    Eigen::MatrixXd R(m, m);
    double covariance = -0.0002;
    double variance = 0.0017;

    R << (variance + var_noise(generator)), (covariance + cov_noise(generator)), (covariance + cov_noise(generator)),
         (covariance + cov_noise(generator)), (variance + var_noise(generator)), (covariance + cov_noise(generator)),
         (covariance + cov_noise(generator)), (covariance + cov_noise(generator)), (variance + var_noise(generator));
    return R;
}

const Eigen::MatrixXd createElevatorMeasurementCovarianceMatrix(unsigned int m)
{
    Eigen::MatrixXd R(m, m);
    R << 0.0085, 0.0014, 0.0025,
         0.0014, 0.007, -0.004,
         0.0025, -0.004, 0.12;
    return R;
}

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
	*outfile << "arx,ary,arz,acx,acy,acz,vx,vy,vz,sx,sy,sz,sfx,sfy,sfz,vfx,vfy,vfz,afx,afy,afz,t\n";
}

void printToFile(std::ofstream* outfile, DataPoint<NavigationVector>* accRaw, DataPoint<NavigationVector>* accCor,
										 DataPoint<NavigationVector>*    vel, DataPoint<NavigationVector>*    pos,
                                         Eigen::VectorXd* x)
{
	*outfile << accRaw->value[0] << "," << accRaw->value[1] << "," << accRaw->value[2] << ","
	 		 << accCor->value[0] << "," << accCor->value[1] << "," << accCor->value[2] << ","
	 		 <<    vel->value[0] << "," <<    vel->value[1] << "," <<    vel->value[2] << ","
			 <<    pos->value[0] << "," <<    pos->value[1] << "," <<    pos->value[2] << ","
             <<    x[0]          << "," <<    x[1]          << "," <<    x[2]          << ","
             <<    x[3]          << "," <<    x[4]          << "," <<    x[5]          << ","
             <<    x[6]          << "," <<    x[7]          << "," <<    x[8]          << ","
			 << accRaw->timestamp << "\n";
}

int main(int argc, char *argv[])
{
	// System setup
	System::parseArgs(argc, argv);
	System& sys = System::getSystem();
	Logger log(sys.verbose, sys.debug);
	Timer timer;

    double current_time = -1.0;
    std::string filterSetup = "stationary";
    bool writeToFile = (sys.imu_id > 0) || (sys.run_id > 0);

    // Sensor setup
    int i2c = 66;
    Imu* imu = new Imu(log, i2c, 0x08, 0x00);
    ImuData* imuData = new ImuData();

    // Filter setup: dynamics & measurement models + initial estimates
    unsigned int n = 9;
    unsigned int m = 3;
    KalmanMvar kalmanFilter = KalmanMvar(n, m);
    Eigen::MatrixXd A = createStateTransitionMatrix(n, 0.0);
    Eigen::MatrixXd H = Eigen::MatrixXd::Zero(m, n);
    for (unsigned int i = 0; i < m; i++)
    {
        H(i, n - (m - i)) = 1.0;
    }
    Eigen::MatrixXd Q = createStateTransitionCovarianceMatrix(n);

    Eigen::MatrixXd R;
    if (filterSetup == "elevator")
    {
        R = createElevatorMeasurementCovarianceMatrix(m);
    } else if (filterSetup == "stationary")
    {
        R = createStationaryMeasurementCovarianceMatrix(m);
    } else
    {
        R = Eigen::MatrixXd::Zero(m, m);
        for (unsigned int i = 0; i < m; i++)
        {
            R(i, i) = 0.05;
        }
    }


    // log print Q matrix
    /*
    for (unsigned int row = 0; row < n; row++)
    {
        std::string s = "";
        for (unsigned int col = 0; col < n; col++)
        {
            s << ("%+6.5f\t ", Q(row, col));
        }
        log.INFO("FILTER_SETUP Q", s);
    }
    */
    //
    // log print R matrix
    log.INFO("FILTER_SETUP R", "%+6.5f\t %+6.5f\t %+6.5f\t", R(0,0),  R(0,1), R(0,2));
    log.INFO("FILTER_SETUP R", "%+6.5f\t %+6.5f\t %+6.5f\t", R(1,0),  R(1,1), R(1,2));
    log.INFO("FILTER_SETUP R", "%+6.5f\t %+6.5f\t %+6.5f\t", R(2,0),  R(2,1), R(2,2));

    kalmanFilter.setModels(A, Q, H, R);

    // set initial estimates
    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(n);
    Eigen::MatrixXd P0 = createInitialErrorCovarianceMatrix(n);
    kalmanFilter.setInitial(x0, P0);
    

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
        {
            accRaw = queryImuAcceleration(imu, imuData, &timer);
        }
		accCor = DataPoint<NavigationVector>(accRaw.timestamp, accRaw.value - gVector);

        Eigen::MatrixXd A;
        if (current_time == -1.0)
        {
            A = createStateTransitionMatrix(n, 0.0);
        } else
        {
            A = createStateTransitionMatrix(n, (accRaw.timestamp - current_time)/1000000.0);
        }
        current_time = accRaw.timestamp;

        kalmanFilter.update(A);
        Eigen::VectorXd z(m);
        for (unsigned int i = 0; i < m; i++)
        {
            z(i) = accCor.value[i];
        }
        kalmanFilter.filter(z);

		// Intergrate
		velIntegrator.update(accCor);
		posIntegrator.update(vel);

		// Output values
		log.INFO("MAIN", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
	  					accCor.value[0], accCor.value[1], accCor.value[2], 
	  							 vel.value[0], vel.value[1], vel.value[2], 
	  							 pos.value[0], pos.value[1], pos.value[2]);		

        Eigen::VectorXd x = kalmanFilter.getStateEstimate();
		log.INFO("FILTERED", "a_x:%+6.3f  a_y:%+6.3f  a_z:%+6.3f\tv_x:%+6.3f  v_y:%+6.3f  v_z:%+6.3f\tp_x:%+6.3f  p_y:%+6.3f  p_z:%+6.3f\n", 
	  					x(6), x(7), x(8),
                        x(3), x(4), x(5),
                        x(0), x(1), x(2));
		if (writeToFile > 0) 
		{
			printToFile(&outfile, &accRaw, &accCor, &vel, &pos, &x);
		}


		sleep(queryDelay);
	}

    Eigen::MatrixXd P = kalmanFilter.getStateCovariance();
    std::cout << P << std::endl;

    log.INFO("FILTER_SETUP R", "%+6.5f\t %+6.5f\t %+6.5f\t", R(0,0),  R(0,1), R(0,2));
    log.INFO("FILTER_SETUP R", "%+6.5f\t %+6.5f\t %+6.5f\t", R(1,0),  R(1,1), R(1,2));
    log.INFO("FILTER_SETUP R", "%+6.5f\t %+6.5f\t %+6.5f\t", R(2,0),  R(2,1), R(2,2));

	if (writeToFile) outfile.close();

	return 0;
}
