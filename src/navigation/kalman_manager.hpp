/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Header for Kalman filter manager (interface for filter and filter setup)
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

#ifndef NAVIGATION_KALMAN_MANAGER_HPP_
#define NAVIGATION_KALMAN_MANAGER_HPP_

#include <random>
#include <Eigen/Dense>

#include "data/data.hpp"
#include "utils/math/kalman_multivariate.hpp"

using Eigen::MatrixXf;
using Eigen::VectorXf;

namespace hyped
{
    using data::NavigationVector;
    using data::NavigationEstimate;
    using utils::math::KalmanMvar;

    namespace navigation
    {
        class KalmanManager
        {
            public:
                KalmanManager(unsigned int n_, unsigned int m_);
                KalmanManager(unsigned int n_, unsigned int m_, unsigned int k_);
                void setupStationary();
                void setupElevator();
                void updateStateTransitionMatrix(double dt);
                void filter(NavigationVector& z_);
                void filter(NavigationVector& u_, NavigationVector& z_);
                const NavigationEstimate getEstimate();

            private:
                unsigned int    n;
                unsigned int    m;
                unsigned int    k;
                KalmanMvar      kalmanFilter;

                // set initial estimate x and error covariance P
                void setInitialEstimate();

                // create initial error covariance matrix P
                const MatrixXf createInitialErrorCovarianceMatrix();

                // create state transition matrix A
                const MatrixXf createStateTransitionMatrix(double dt);

                // create measurement matrix H
                const MatrixXf createMeasurementMatrix();

                // create state transition coveriance matrix Q
                const MatrixXf createStateTransitionCovarianceMatrix();

                // create measurement covariance matrices R
                const MatrixXf createStationaryMeasurementCovarianceMatrix();
                const MatrixXf createElevatorMeasurementCovarianceMatrix();
        };
    }
}


#endif  // NAVIGATION_KALMAN_MANAGER_HPP_
