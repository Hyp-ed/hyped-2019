/*
 * Author: Neil Weidinger
 * Organisation: HYPED
 * Date: March 2019
 * Description:
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

#include "telemetry/main.hpp"
#include "telemetry/sendloop.hpp"
#include "telemetry/recvloop.hpp"

namespace hyped {

namespace telemetry {

Main::Main(uint8_t id, Logger& log)
  : Thread {id, log},
    client_ {log}
{
  log_.DBG("Telemetry", "Telemetry Main thread object created");
}

void Main::run()
{
  log_.DBG("Telemetry", "Telemetry Main thread started");

  if (!client_.connect()) {
    // idk throw exception or something
    log_.ERR("Telemetry", "ERROR CONNECTING TO SERVER");
  }

  SendLoop sendloop_thread {log_, this};  // NOLINT
  RecvLoop recvloop_thread {log_, this};  // NOLINT

  sendloop_thread.start();
  recvloop_thread.start();

  sendloop_thread.join();
  recvloop_thread.join();
}

}  // namespace telemetry
}  // namespace hyped
