/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 31/03/2019
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

#include "kalman_multivariate.hpp"

namespace hyped {
    namespace utils {
        namespace math {

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

            void KalmanMvar::setDynamicsModel(MatrixXf& _A, MatrixXf& _Q)
            {
                A = _A;
                Q = _Q;
            }

            void KalmanMvar::setDynamicsModel(MatrixXf& _A, MatrixXf& _B, MatrixXf& _Q)
            {
                A = _A;
                B = _B;
                Q = _Q;
            }

            void KalmanMvar::setMeasurementModel(MatrixXf& _H, MatrixXf& _R)
            {
                H = _H;
                R = _R;
            }

            void KalmanMvar::setModels(MatrixXf& _A, MatrixXf& _Q, MatrixXf& _H,
                                       MatrixXf& _R)
            {
                setDynamicsModel(_A, _Q);
                setMeasurementModel(_H, _R);
            }

            void KalmanMvar::setModels(MatrixXf& _A, MatrixXf& _B, MatrixXf& _Q,
                                       MatrixXf& _H, MatrixXf& _R)
            {
                setDynamicsModel(_A, _B, _Q);
                setMeasurementModel(_H, _R);
            }

            void KalmanMvar::update(MatrixXf& _A)
            {
                A = _A;
            }

            void KalmanMvar::setInitial(VectorXf& x0, MatrixXf& P0)
            {
                x = x0;
                P = P0;
                I = MatrixXf::Identity(n, n);
            }

            void KalmanMvar::predict()
            {
                x = A * x;
                P = A * P * A.transpose() + Q;
            }

            void KalmanMvar::predict(VectorXf& u)
            {
                x = A * x + B * u;
                P = (A * P * A.transpose()) + Q;
            }

            void KalmanMvar::correct(VectorXf& z)
            {
                MatrixXf K = (P * H.transpose()) * (H * P * H.transpose() + R).inverse();
                x = x + K * (z - H * x);
                P = (I - K * H) * P;
            }

            void KalmanMvar::filter(VectorXf& z)
            {
                predict();
                correct(z);
            }

            void KalmanMvar::filter(VectorXf& u, VectorXf& z)
            {
                predict(u);
                correct(z);
            }

            VectorXf& KalmanMvar::getStateEstimate()
            {
                return x;
            }

            MatrixXf& KalmanMvar::getStateCovariance()
            {
                return P;
            }

        }
    }
}
