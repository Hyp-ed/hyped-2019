/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 09/03/2019
 * Description: Multivariate Kalman filter implementation to filter sensor measurement
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

#ifndef UTILS_MATH_KALMAN_MULTIVARIATE_HPP_
#define UTILS_MATH_KALMAN_MULTIVARIATE_HPP_

#include <Eigen/Dense>

using Eigen::MatrixXf;
using Eigen::VectorXf;

namespace hyped {
    namespace utils {
        namespace math {

            /**
             * @brief    This class is for filtering the data from sensors to smoothen it.
             */
            class KalmanMultivariate {
             public:
              /**
               * @brief    Construct a new Kalman object with respective dimensions (without control)
               *
               * @param[in] n                       state dimensionality
               * @param[in] m                       measurement dimensionality
               */
              KalmanMultivariate(unsigned int _n, unsigned int _m);

              /**
               * @brief    Construct a new Kalman object with respective dimensions (with control)
               *
               * @param[in] n                       state dimensionality
               * @param[in] m                       measurement dimensionality
               * @param[in] k                       control dimensionality
               */
              KalmanMultivariate(unsigned int _n, unsigned int _m, unsigned int _k);

              /**
               * @brief    Set dynamics model matrices (without control)
               *
               * @param[in] A                       state transition matrix
               * @param[in] Q                       process noise covariance
               */
              void setDynamicsModel(MatrixXf& _A, MatrixXf& _Q);

              /**
               * @brief    Set dynamics model matrices (with control)
               *
               * @param[in] A                       state transition matrix
               * @param[in] B                       control matrix
               * @param[in] Q                       process noise covariance
               */
              void setDynamicsModel(MatrixXf& _A, MatrixXf& _B, MatrixXf& _Q);

              /**
               * @brief    Set measurement model matrices
               *
               * @param[in] H                       measurement matrix
               * @param[in] R                       measurement noise covariance
               */
              void setMeasurementModel(MatrixXf& _H, MatrixXf& _R);

              /**
               * @brief    Set model matrices (without control)
               *
               * @param[in] A                       state transition matrix
               * @param[in] Q                       process noise covariance
               * @param[in] H                       measurement matrix
               * @param[in] R                       measurement noise covariance
               */
              void setModels(MatrixXf& _A, MatrixXf& _Q, MatrixXf& _H,
                             MatrixXf& _R);

              /**
               * @brief    Set model matrices (with control)
               *
               * @param[in] A                       state transition matrix
               * @param[in] B                       control matrix
               * @param[in] Q                       process noise covariance
               * @param[in] H                       measurement matrix
               * @param[in] R                       measurement noise covariance
               */
              void setModels(MatrixXf& _A, MatrixXf& _B, MatrixXf& _Q,
                             MatrixXf& _H, MatrixXf& _R);

              /**
               * @brief    Update state transition matrix
               *
               * @param[in] A                       state transition matrix
               */
              void update(MatrixXf& _A);

              /**
               * @brief    Set initial beliefs
               *
               * @param[in] x0                      initial state belief
               * @param[in] P0                      initial state covariance (uncertainty)
               */
              void setInitial(VectorXf& x0, MatrixXf& P0);

              /**
               * @brief    Filter measurement and update state belief with covariance (without control)
               *
               * @param[in] z                       measurement vector
               */
              void filter(VectorXf& z);

              /**
               * @brief    Filter measurement and update state belief with covariance (with control)
               *
               * @param[in] u                       control vector
               * @param[in] z                       measurement vector
               */
              void filter(VectorXf& u, VectorXf& z);

              /**
               * @brief     Get the state estimate
               *
               * @return    Returns the current state estimate
               */
              VectorXf& getStateEstimate();

              /**
               * @brief     Get the state uncertainty
               *
               * @return    Returns the current state covariance
               */
              MatrixXf& getStateCovariance();

             private:
              /* problem dimensions */
              unsigned int n;                 // state dimension
              unsigned int m;                 // measurement dimension
              unsigned int k;                 // control dimension (0 if not set)

              /* dynamics model matrices */
              MatrixXf A;                    // state transition matrix: n x n
              MatrixXf B;                    // control matrix: n x k
              MatrixXf Q;                    // process noise covariance: n x n

              /* measurement model matrices */
              MatrixXf H;                    // measurement matrix: m x n
              MatrixXf R;                    // measurement noise covariance: m x m

              /* state estimates */
              VectorXf x;                    // state vector: n x 1
              MatrixXf P;                    // state covariance: n x n
              MatrixXf I;                    // identity matrix: n x n

              /**
               * @brief    Predict state belief with covariance based on dynamics (without control)
               */
              void predict();

              /**
               * @brief    Predict state belief with covariance based on dynamics (with control)
               *
               * @param[in] u                       control vector
               */
              void predict(VectorXf& u);

              /**
               * @brief    Correct state belief with covariance based on measurement
               *
               * @param[in] z                       measurement vector
               */
              void correct(VectorXf& z);
            };
        }
    }
}

#endif  // UTILS_MATH_KALMAN_MULTIVARIATE_HPP_
