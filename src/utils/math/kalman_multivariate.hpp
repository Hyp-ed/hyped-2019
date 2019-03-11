/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 9 March 2019
 * Description: This is a multivariate Kalman filter implementation to filter sensor measurement
 *              considering the dynamics of the system
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

#include <Eigen/Dense>


#ifndef BEAGLEBONE_BLACK_UTILS_MATH_KALMAN_MVAR_HPP_
#define BEAGLEBONE_BLACK_UTILS_MATH_KALMAN_MVAR_HPP_

namespace hyped {
namespace utils {
namespace math {

/**
 * @brief    This class is for filtering the data from sensors to smoothen it.
 */
class KalmanMvar {
 public:
  /**
   * @brief    Construct a new Kalman object with respective dimensions (without control)
   *
   * @param[in] n                       state dimensionality
   * @param[in] m                       measurement dimensionality
   */
  KalmanMvar(unsigned int _n, unsigned int _m);

  /**
   * @brief    Construct a new Kalman object with respective dimensions (with control)
   *
   * @param[in] n                       state dimensionality
   * @param[in] m                       measurement dimensionality
   * @param[in] k                       control dimensionality
   */
  KalmanMvar(unsigned int _n, unsigned int _m, unsigned int _k);

  /**
   * @brief    Set dynamics model matrices (without control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] Q                       process noise covariance
   */
  void setDynamicsModel(Eigen::MatrixXd& _A, Eigen::MatrixXd& _Q);

  /**
   * @brief    Set dynamics model matrices (with control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] B                       control matrix
   * @param[in] Q                       process noise covariance
   */
  void setDynamicsModel(Eigen::MatrixXd& _A, Eigen::MatrixXd& _B, Eigen::MatrixXd& _Q);

  /**
   * @brief    Set measurement model matrices
   *
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setMeasurementModel(Eigen::MatrixXd& _H, Eigen::MatrixXd& _R);

  /**
   * @brief    Set model matrices (without control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] Q                       process noise covariance
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setModels(Eigen::MatrixXd& _A, Eigen::MatrixXd& _Q, Eigen::MatrixXd& _H, Eigen::MatrixXd& _R);

  /**
   * @brief    Set model matrices (with control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] B                       control matrix
   * @param[in] Q                       process noise covariance
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setModels(Eigen::MatrixXd& _A, Eigen::MatrixXd& _B, Eigen::MatrixXd& _Q, Eigen::MatrixXd& _H, Eigen::MatrixXd& _R);

  /**
   * @brief    Update state transition matrix
   *
   * @param[in] A                       state transition matrix
   */
  void update(Eigen::MatrixXd& _A);

  /**
   * @brief    Set initial beliefs
   *
   * @param[in] x0                      initial state belief
   * @param[in] P0                      initial state covariance (uncertainty)
   */
  void setInitial(Eigen::VectorXd& x0, Eigen::MatrixXd& P0);

  /**
   * @brief    Filter measurement and update state belief with covariance (without control)
   *
   * @param[in] z                       measurement vector
   */
  void filter(Eigen::VectorXd& z);

  /**
   * @brief    Filter measurement and update state belief with covariance (with control)
   *
   * @param[in] u                       control vector
   * @param[in] z                       measurement vector
   */
  void filter(Eigen::VectorXd& u, Eigen::VectorXd& z);

  /**
   * @brief     Get the state estimate
   *
   * @return    Returns the current state estimate
   */
  Eigen::VectorXd& getStateEstimate();

  /**
   * @brief     Get the state uncertainty
   *
   * @return    Returns the current state covariance
   */
  Eigen::MatrixXd& getStateCovariance();

 private:
  /* problem dimensions */
  const unsigned int n;                 // state dimension
  const unsigned int m;                 // measurement dimension
  const unsigned int k;                 // control dimension (0 if not set)

  /* dynamics model matrices */
  Eigen::MatrixXd A;                    // state transition matrix: n x n
  Eigen::MatrixXd B;                    // control matrix: n x k
  Eigen::MatrixXd Q;                    // process noise covariance: n x n

  /* measurement model matrices */
  Eigen::MatrixXd H;                    // measurement matrix: m x n
  Eigen::MatrixXd R;                    // measurement noise covariance: m x m

  /* state estimates */
  Eigen::VectorXd x;                    // state vector: n x 1
  Eigen::MatrixXd P;                    // state covariance: n x n
  Eigen::MatrixXd I;                    // identity matrix: n x n

  /**
   * @brief    Predict state belief with covariance based on dynamics (without control) 
   */
  void predict();

  /**
   * @brief    Predict state belief with covariance based on dynamics (with control) 
   *
   * @param[in] u                       control vector
   */
  void predict(Eigen::VectorXd& u);

  /**
   * @brief    Correct state belief with covariance based on measurement
   *
   * @param[in] z                       measurement vector
   */
  void correct(Eigen::VectorXd& z);
};


KalmanMvar::KalmanMvar(unsigned int _n, unsigned int _m)
    : n(_n),
      m(_m),
      k(0)
{}

KalmanMvar::KalmanMvar(unsigned int _n, unsigned int _m, unsigned int _k)
    : n(_n),
      m(_m),
      k(_k)
{}

void KalmanMvar::setDynamicsModel(Eigen::MatrixXd& _A, Eigen::MatrixXd& _Q)
{
    A = _A;
    Q = _Q;
}

void KalmanMvar::setDynamicsModel(Eigen::MatrixXd& _A, Eigen::MatrixXd& _B, Eigen::MatrixXd& _Q)
{
    A = _A;
    B = _B;
    Q = _Q;
}

void KalmanMvar::setMeasurementModel(Eigen::MatrixXd& _H, Eigen::MatrixXd& _R)
{
    H = _H;
    R = _R;
}

void KalmanMvar::setModels(Eigen::MatrixXd& _A, Eigen::MatrixXd& _Q, Eigen::MatrixXd& _H, Eigen::MatrixXd& _R)
{
    setDynamicsModel(_A, _Q);
    setMeasurementModel(_H, _R);
}

void KalmanMvar::setModels(Eigen::MatrixXd& _A, Eigen::MatrixXd& _B, Eigen::MatrixXd& _Q, Eigen::MatrixXd& _H, Eigen::MatrixXd& _R)
{
    setDynamicsModel(_A, _B, _Q);
    setMeasurementModel(_H, _R);
}

void KalmanMvar::update(Eigen::MatrixXd& _A)
{
    A = _A;
}

void KalmanMvar::setInitial(Eigen::VectorXd& x0, Eigen::MatrixXd& P0)
{
    x = x0;
    P = P0;
    I = Eigen::MatrixXd::Identity(n, n);
}

void KalmanMvar::predict()
{
    x = A * x;
    P = A * P * A.transpose() + Q;
}

void KalmanMvar::predict(Eigen::VectorXd& u)
{
    x = A * x + B * u;
    P = (A * P * A.transpose()) + Q;
}

void KalmanMvar::correct(Eigen::VectorXd& z)
{
    Eigen::MatrixXd K = (P * H.transpose()) * (H * P * H.transpose() + R).inverse();
    x = x + K * (z - H * x);
    P = (I - K * H) * P;
}

void KalmanMvar::filter(Eigen::VectorXd& z)
{
    predict();
    correct(z);
}

void KalmanMvar::filter(Eigen::VectorXd& u, Eigen::VectorXd& z)
{
    predict(u);
    correct(z);
}

Eigen::VectorXd& KalmanMvar::getStateEstimate()
{
    return x;
}

Eigen::MatrixXd& KalmanMvar::getStateCovariance()
{
    return P;
}

}}}  // hyped::util::math

#endif  // BEAGLEBONE_BLACK_UTILS_MATH_KALMAN_MVAR_HPP_
