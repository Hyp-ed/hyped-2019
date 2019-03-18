
#include "sensors/fake_imu.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "data/data.hpp"

using hyped::utils::System;
using hyped::utils::Logger;
using hyped::data::NavigationType;
using hyped::data::NavigationVector;
using hyped::ImuData;
using hyped::data::Data;
using hyped::sensors::FakeImuFromFile;


int main(int argc, char *argv[]) {
    hyped::utils::System::parseArgs(argc, argv);
    Logger log = System::getLogger();

    std::string acc_file_path, dec_file_path, em_file_path;
    acc_file_path = "/home/mac/Documents/UoE/hyped/hyped-2018/BeagleBone_black/data/in/fake_imu_input_acc.txt";
    dec_file_path = "/home/mac/Documents/UoE/hyped/hyped-2018/BeagleBone_black/data/in/fake_imu_input_dec.txt";
    em_file_path = "/home/mac/Documents/UoE/hyped/hyped-2018/BeagleBone_black/data/in/fake_imu_input_em.txt";

    // Data data = Data::getInstance();
    ImuData imu;
    FakeImuFromFile fake_imu(log,acc_file_path,dec_file_path,em_file_path);
    fake_imu.getData(&imu);
    NavigationVector naviationData = imu.acc;
    for (int i = 0; i < 7; i++){
        NavigationType d = naviationData[i];
        log.INFO("IMU_DATA", "Acc %f",d);
    }
}
