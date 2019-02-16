

#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include <vector>

using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using namespace std;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);
  Imu imu(log, 66, 0x08, 0x00);

  log.INFO("TEST-Imu", "MPU9250 instance successfully created");
  // int count = imu.readFifo(data);
  // log.DBG("FIFO_COUNT", "%d", count);
  while (true) {
    std::vector<ImuData> data;
    int count = imu.readFifo(data);
    log.DBG("ReadFifo Count", "%d", count);
    for (int i=0; i < count; i++) {
      log.DBG("Loop Count", "%d", i);
      // log.DBG("TEST-mpu9250", "accelerometer readings x: %f m/s^2, y: %f m/s^2, z: %f m/s^2", data[i].acc[0], data[i].acc[1], data[i].acc[2]);
      // log.DBG("TEST-mpu9250", "gyroscope readings     x: %f rad/s, y: %f rad/s, z: %f rad/s", data[i].gyr[0], data[i].gyr[1], data[i].gyr[2]);
    }
    Thread::sleep(1000);
  }
  

  // for (int i=0; i< 100; i++) {
  //   log.DBG("TEST-mpu9250", "accelerometer readings x: %f m/s^2, y: %f m/s^2, z: %f m/s^2", imu.acc[0], imu.acc[1], imu.acc[2]);
  //   log.DBG("TEST-mpu9250", "gyroscope readings     x: %f rad/s, y: %f rad/s, z: %f rad/s", imu.gyr[0], imu.gyr[1], imu.gyr[2]);
  //   Thread::sleep(500);
  // }

 	return 0;
}
