/*
 * 
 * Description: Main class for fake IMUs
 * 
 */

#include "data/data.hpp"
#include "sensors/interface.hpp"
#include "utils/logger.hpp"

namespace hyped {

using data::ImuData;
using data::DataPoint;
using data::NavigationType;
using data::NavigationVector;

namespace sensors {
class FakeImu : public ImuInterface {
    public:
        /**
         * @brief   Construcor for Fake Imu class
         * 
         */
        FakeImu(utils::Logger& log_,
                std::string acc_filepath,
                std::string dec_filepath,
                std::string em_filepath);

        bool isOnline() override { return true; }

        void getData(ImuData* imu_data);

    private:
        utils::Logger& log_;
        const uint64_t kAccTimeInterval = 50;

        void startAcc();
        void startDec();
        void startEm();
        void readFileData(std::string acc_filepath,
                          std::string dec_filepath,
                          std::string em_filepath);

};
}} // hyped::sensors