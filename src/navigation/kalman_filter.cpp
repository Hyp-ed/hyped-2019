/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Kalman filter (interface for filter and filter setup)
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

#include "kalman_filter.hpp"

namespace hyped {
namespace navigation {

KalmanFilter::KalmanFilter(unsigned int n_/*=3*/, unsigned int m_/*=1*/, unsigned int k_/*=0*/)
  : n(n_),
    m(m_),
    k(k_),
    kalmanFilter(KalmanMultivariate(n_, m_, k_))
{}

void KalmanFilter::setup()
{
  // setup dynamics & measurement models for stationary test
  MatrixXf A = createStateTransitionMatrix(0.0);
  MatrixXf Q = createStateTransitionCovarianceMatrix();
  MatrixXf H = createMeasurementMatrix();

  // check system navigation run for R setup
  System &sys = System::getSystem();
  MatrixXf R = MatrixXf::Zero(m, m);;
  if (sys.tube_run) R = createTubeMeasurementCovarianceMatrix();
  else if (sys.elevator_run) R = createElevatorMeasurementCovarianceMatrix();
  else if (sys.stationary_run) R = createStationaryMeasurementCovarianceMatrix();

  kalmanFilter.setModels(A, Q, H, R);

  // setup initial estimates
  VectorXf x = VectorXf::Zero(n);
  MatrixXf P = createInitialErrorCovarianceMatrix();
  kalmanFilter.setInitial(x, P);
}

void KalmanFilter::updateStateTransitionMatrix(double dt)
{
  MatrixXf A = createStateTransitionMatrix(dt);
  kalmanFilter.update(A);
}

const NavigationVector KalmanFilter::filter(NavigationType z_)
{
  VectorXf z(m);
  z(0) = z_;
  kalmanFilter.filter(z);

  NavigationVector estimate = getNavigationVector();
  return estimate;
}

const NavigationVector KalmanFilter::filter(NavigationType u_, NavigationType z_)
{
  VectorXf u(k);
  u(0) = u_;

  VectorXf z(m);
  z(0) = z_;

  kalmanFilter.filter(u, z);

  NavigationVector estimate = getNavigationVector();
  return estimate;
}

const MatrixXf KalmanFilter::createInitialErrorCovarianceMatrix()
{
  MatrixXf P = MatrixXf::Constant(n, n, 0.0);
  std::default_random_engine generator;
  std::normal_distribution<double> pos_var_noise(0.0, 0.001);
  std::normal_distribution<double> vel_var_noise(0.0, 0.005);
  std::normal_distribution<double> acc_var_noise(0.0, 0.01);

  for (unsigned int i = 0; i< n; i++) {
    if (i < n/3) {
      P(i, i) = pos_var_noise(generator);
    } else if (i < 2*n/3) {
      P(i, i) = vel_var_noise(generator);
    } else {
      P(i, i) = acc_var_noise(generator);
    }
  }
  return P;
}

void KalmanFilter::setInitialEstimate()
{
  // create initial error covariance matrix P
  MatrixXf P = MatrixXf::Constant(n, n, 0.0);
  std::default_random_engine generator;
  std::normal_distribution<double> pos_var_noise(0.0, 0.001);
  std::normal_distribution<double> vel_var_noise(0.0, 0.005);
  std::normal_distribution<double> acc_var_noise(0.0, 0.01);

  for (unsigned int i = 0; i< n; i++) {
    if (i < n/3) {
      P(i, i) = pos_var_noise(generator);
    } else if (i < 2*n/3) {
      P(i, i) = vel_var_noise(generator);
    } else {
      P(i, i) = acc_var_noise(generator);
    }
  }

  // create initial estimate x
  VectorXf x = VectorXf::Zero(n);
  kalmanFilter.setInitial(x, P);
}

const MatrixXf KalmanFilter::createStateTransitionMatrix(double dt)
{
  MatrixXf A = MatrixXf::Zero(n, n);
  double acc_ddt = 0.5 * dt * dt;
  //  number of values for each acc, vel, pos: usually 1 or 3
  unsigned int n_val = n / 3;

  for (unsigned int i = 0; i < n_val; i++) {
      // compute pos rows
      A(i, i) = 1.;
      A(i, i + n_val) = dt;
      A(i, i + 2 * n_val) = acc_ddt;

      // compute vel rows
      A(i + n_val, i + n_val) = 1.;
      A(i + n_val, i + 2 * n_val) = dt;

      // compute acc rows
      A(i + 2 * n_val, i + 2 * n_val) = 1.;
  }

  return A;
}

const MatrixXf KalmanFilter::createMeasurementMatrix()
{
  MatrixXf H = MatrixXf::Zero(m, n);
  for (unsigned int i = 0; i < m; i++) {
    H(i, n - (m - i)) = 1.0;
  }
  return H;
}

const MatrixXf KalmanFilter::createStateTransitionCovarianceMatrix()
{
  std::default_random_engine generator;
  std::normal_distribution<double> var_noise(0.01, 0.02);

  MatrixXf Q = MatrixXf::Constant(n, n, 0.0);
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

const MatrixXf KalmanFilter::createStationaryMeasurementCovarianceMatrix()
{
  std::default_random_engine generator;
  std::normal_distribution<double> var_noise(0.0, 0.0005);
  std::normal_distribution<double> cov_noise(0.0, 0.0001);

  MatrixXf R = MatrixXf::Zero(m, m);
  double covariance = -0.0002;
  double variance = 0.0017;

  for (unsigned int i = 0; i < m; i++) {
      for (unsigned int j = 0; j < m; j++) {
          if (i == j) {
              R(i, j) = variance + var_noise(generator);
          } else {
              R(i, j) = covariance + cov_noise(generator);
          }
      }
  }

  return R;
}

const MatrixXf KalmanFilter::createTubeMeasurementCovarianceMatrix()
{
    // TODO(Lukas): implement proper covariance setup for tube based on simulation
    MatrixXf R = MatrixXf::Zero(m, m);
    return R;
}
const MatrixXf KalmanFilter::createElevatorMeasurementCovarianceMatrix()
{
  MatrixXf R(m, m);
  /*
  R << 0.0085, 0.0014, 0.0025,
       0.0014, 0.007, -0.004,
       0.0025, -0.004, 0.12;
   */
  R << 0.12;
  return R;
}

const NavigationVector KalmanFilter::getNavigationVector()
{
  VectorXf x = kalmanFilter.getStateEstimate();
  NavigationVector est = NavigationVector({x(0), x(1), x(2)});
  return est;
}
}}  // namespace hyped navigation
