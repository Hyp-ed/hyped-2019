/*
 * Author:
 * Organisation: HYPED
 * Date:
 * Description: Demo for LM35 Thermistor using temp_manager
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

#include "sensors/temp_manager.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;
using namespace hyped::data;
using namespace std;
using hyped::sensors::TempManager;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, -1);

  Data& data_ = Data::getInstance();    // read from data struct
  TempManager temp_manager_(log);
  temp_manager_.start();
  Thread::sleep(500);
  
  log.INFO("TEST-Temp", "Temp instance successfully created");
  for (int i = 0; i < 50; i++) {
    log.INFO("TEST-Temp", "Thermistor %d reading: %d degrees C", i, data_.getTemperature());
    Thread::sleep(100);
  }
 	return 0;
}
