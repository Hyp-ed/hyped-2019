/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description:
 * Main instantiates HypedMachine. It also monitors other data and generates Events
 * for the HypedMachine. Note, StateMachine structure in Data is not updated here but
 * in HypedMachine.
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "sensors/imu.hpp"
#include "utils/timer.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

#include <vector>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <cstdlib>

using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::utils::ScopedTimer;
using hyped::utils::concurrent::Thread;
using hyped::data::ImuData;
using hyped::data::NavigationType;
using hyped::data::NavigationVector;

void sensorAverage(NavigationVector &acc, NavigationVector &gyr, std::vector<ImuData *> & msmnts)
{
  float nSensors = float(msmnts.size());
  for (ImuData *msmnt : msmnts)
  {
    acc[0] += msmnt->acc[0] / nSensors;
    acc[1] += msmnt->acc[1] / nSensors;
    acc[2] += msmnt->acc[2] / nSensors;

    gyr[0] += msmnt->gyr[0] / nSensors;
    gyr[1] += msmnt->gyr[1] / nSensors;
    gyr[2] += msmnt->gyr[2] / nSensors;
  }
}


float absoluteSum(NavigationVector &v)
{
    float absSum = 0.0;
    for (unsigned int i = 0; i < 3; i++) {
        absSum += abs(v[i]);
    }
    return absSum;
}


NavigationVector computeAvgAcc(unsigned int nSensors, std::vector<Imu *> &sensors,
                                  std::vector<ImuData *> &imus, unsigned int measurements)
{
    std::vector<NavigationVector> accelerations(measurements);
    for (unsigned int i = 0; i < measurements; i++)
      {
          NavigationVector acc({0., 0., 0.});
          NavigationVector gyr({0., 0., 0.});
          // get data from IMUs to MPU sensors
          for (unsigned int j = 0; j < nSensors; ++j)
          {
            sensors[j]->getData(imus[j]);
          }
          sensorAverage(acc, gyr, imus);
          accelerations[i] = acc;

          sleep(1.0/float(measurements));
      }

      // compute the average of 100 accs for gravity
      NavigationVector acc_gravity({0., 0., 0.});
      for (unsigned int i = 0; i < measurements; i++)
      {
          for (int j = 0; j < 3; j++)
          {
              acc_gravity[j] += accelerations[i][j]/float(measurements);
          }
      }
      return acc_gravity;
}


int main(int argc, char* argv[])
{
  // Some intial parameters
  // number of units used -> remember to set i2cs vector
  unsigned int nSensors = 1;
  // number of iterations or -1 for infinite iterations
  unsigned int nQueries = 100;
  double last_time = 0.0;

  // System setup
  hyped::utils::System::parseArgs(argc, argv);
  Logger& log = hyped::utils::System::getLogger();

  // Initialise array of sensors
  std::vector<Imu *> sensors(nSensors);
  std::vector<ImuData *> imus(nSensors);
  // need to set these values manually
  std::vector<int> i2cs = {48};  // i2c locations of sensors

  assert(nSensors == i2cs.size());
  for (unsigned int i = 0; i < nSensors; ++i)
  {
    Imu * mpu = new Imu(log, i2cs[i], 0x08, 0x00);
    ImuData * imu = new ImuData();
    sensors[i] = mpu;
    imus[i] = imu;
  }

  // compute gravity acceleration given current orientation
  // get 100 measurements
  unsigned int grav_measurements = 100;
  NavigationVector acc_gravity = computeAvgAcc(nSensors, sensors, imus, grav_measurements);
  // recompute gravity until the value seems reasonable
  float absSum = absoluteSum(acc_gravity);
  while (absSum < 9.5 || absSum > 10.0)
  {
      log.INFO("Gravity Acceleration", "Sum of absolute values is %f which is not plausible -> recompute gravity acceleration", absSum);
      sleep(0.5);
      acc_gravity = computeAvgAcc(nSensors, sensors, imus, grav_measurements);
  }

  log.INFO("Gravity Acceleration", "x: %f m/s^2, y: %f m/s^2, z: %f m/s^2",
          acc_gravity[0], acc_gravity[1], acc_gravity[2]);

  // Perform specified number of measurements
  Timer timer;
  double dt;
  NavigationVector vel({0., 0., 0.});
  unsigned int query = 0;
  //while ((query < nQueries) || (nQueries = -1)) {
  for (unsigned int i = 0; i < nQueries; i++) {
    // Measure acceleration
    NavigationVector acc({0., 0., 0.});
    NavigationVector gyr({0., 0., 0.});
    // New scope to generate dt
    {
      ScopedTimer scope_timer(&timer);

      // get data from IMUs to MPU sensors
      for (unsigned int j = 0; j < nSensors; ++j)
      {
        sensors[j]->getData(imus[j]);
      }
    }
    // time stamp in seconds
    double current_time = timer.getMillis();
    dt = (current_time - last_time)/ 1000.0;
    last_time = current_time;

    // Determine velocity
    {
      ScopedTimer scope_timer(&timer);

      sensorAverage(acc, gyr, imus);
      for (unsigned int j = 0; j < 3; j++)
      {
          acc[j] -= acc_gravity[j];
          vel[j] += acc[j] * dt;
      }
    }


    // Print
    log.INFO("TEST-mpu9250", "acceleration readings x: %f m/s^2, y: %f m/s^2, z: %f m/s^2",
                                                                     acc[0], acc[1], acc[2]);
    log.INFO("TEST-mpu9250", "velocity readings x: %f m/s, y: %f m/s, z: %f m/s\tblind time: %f\n",
                                                                     vel[0], vel[1], vel[2], dt);

    query++;
  }

  // cleanup
  for (unsigned int i = 0; i < nSensors; i++)
  {
    delete imus[i];
  }

}
