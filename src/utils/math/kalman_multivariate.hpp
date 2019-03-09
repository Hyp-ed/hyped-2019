/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 9 March 2019
 * Description: This is a multivariate Kalman filter implementation to filter sensor measurement
 *              considering the dynamics of the system
 */

#include <Eigen/Dense>


#ifndef BEAGLEBONE_BLACK_UTILS_MATH_KALMAN_HPP_
#define BEAGLEBONE_BLACK_UTILS_MATH_KALMAN_HPP_

namespace hyped {
namespace utils {
namespace math {

/**
 * @brief    This class is for filtering the data from sensors to smoothen it.
 */
class Kalman_mvar {
 public:
  /**
   * @brief    Construct a new Kalman object with respective dimensions (without control)
   *
   * @param[in] n                       state dimensionality
   * @param[in] m                       measurement dimensionality
   */
  Kalman(unsigned int n, unsigned int m);

  /**
   * @brief    Construct a new Kalman object with respective dimensions (with control)
   *
   * @param[in] n                       state dimensionality
   * @param[in] m                       measurement dimensionality
   * @param[in] k                       control dimensionality
   */
  Kalman(unsigned int n, unsigned int m, unsigned int k);

  /**
   * @brief    Set dynamics model matrices (without control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] Q                       process noise covariance
   */
  void setDynamicsModel(Eigen::MatrixXd A, Eigen::MatrixXd Q);

  /**
   * @brief    Set dynamics model matrices (with control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] B                       control matrix
   * @param[in] Q                       process noise covariance
   */
  void setDynamicsModel(Eigen::MatrixXd A, Eigen::MatrixXd B, Eigen::MatrixXd Q);

  /**
   * @brief    Set measurement model matrices
   *
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setMeasurementModel(Eigen::MatrixXd H, Eigen::MatrixXd R);

  /**
   * @brief    Set model matrices (without control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] Q                       process noise covariance
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setModels(Eigen::MatrixXd A, Eigen::MatrixXd Q, Eigen::MatrixXd H, Eigen::MatrixXd R);

  /**
   * @brief    Set model matrices (with control)
   *
   * @param[in] A                       state transition matrix
   * @param[in] B                       control matrix
   * @param[in] Q                       process noise covariance
   * @param[in] H                       measurement matrix
   * @param[in] R                       measurement noise covariance
   */
  void setModels(Eigen::MatrixXd A, Eigen::MatrixXd B, Eigen::MatrixXd Q, Eigen::MatrixXd H, Eigen::MatrixXd R);

  /**
   * @brief    Set initial beliefs
   *
   * @param[in] x0                      initial state belief
   * @param[in] P0                      initial state covariance (uncertainty)
   */
  void setInitial(Eigen::VectorXf x0, Eigen::MatrixXd P0);

  /**
   * @brief    Filter measurement and update state belief with covariance (without control)
   *
   * @param[in] z                       measurement vector
   */
  void filter(Eigen::VectorXf z);

  /**
   * @brief    Filter measurement and update state belief with covariance (with control)
   *
   * @param[in] u                       control vector
   * @param[in] z                       measurement vector
   */
  void filter(Eigen::VectorXf u, Eigen::VectorXf z);

  /**
   * @brief     Get the state estimate
   *
   * @return    Returns the current state estimate
   */
  Eigen::VectorXf getStateEstimate();

  /**
   * @brief     Get the state uncertainty
   *
   * @return    Returns the current state covariance
   */
  Eigen::MatrixXd getStateCovariance();

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
  Eigen::VectorXf x;                    // state vector: n x 1
  Eigen::MatrixXd P;                    // state covariance: n x n

  /**
   * @brief    Predict state belief with covariance based on dynamics (without control) 
   */
  void predict();

  /**
   * @brief    Predict state belief with covariance based on dynamics (with control) 
   *
   * @param[in] u                       control vector
   */
  void predict(Eigen::VectorXf u);

  /**
   * @brief    Correct state belief with covariance based on measurement
   *
   * @param[in] z                       measurement vector
   */
  void correct(Eigen::VectorXf z);
};

}}}  // hyped::util::math

#endif  // BEAGLEBONE_BLACK_UTILS_MATH_KALMAN_HPP_
