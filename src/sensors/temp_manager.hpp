#ifndef SENSORS_TEMP_MANAGER_HPP_
#define SENSORS_TEMP_MANAGER_HPP_

#include <cstdint>

#include "sensors/manager_interface.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "sensors/interface.hpp"
#include "utils/system.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;

namespace sensors {

class TempManager: public Thread {
  typedef array<AIN*, data::Sensors::kNumThermistors>  DataArray;      // kNumThermistors

 public:
  TempManager(Logger& log);
  void run()            override;
 private:
  utils::System&   sys_;
  DataArray        temp_;
  data::Data&      data_;
  utils::Logger&   log_;
  // TempInterface*   temp_[data::Sensors::kNumThermistors];  // TODO(anyone): need to implement?
};

}}    // namespace hyped::sensors

#endif     // SENSORS_TEMP_MANAGER_HPP_ 