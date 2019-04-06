/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 3. April 2019
 * Description:
 *
 * This a configuration class that should hold all necessary data to configure
 * different submodules in the system.
 * E.g. for telemetry, IP address to be used can be configured
 *
 * Configuration values are read from a configuration file provided.
 *
 *    Copyright 2018 HYPED
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
#ifndef UTILS_CONFIG_HPP_
#define UTILS_CONFIG_HPP_

namespace hyped {
namespace utils {

// forward declaration
class System;
class ModuleEntry;

enum Submodule {
  kNone,
  kNavigation,
  kTelemetry,
  kPropulsion,
  kSensors,
  kStateMachine
};

class Config {
  friend System;    // ensure System can access our private parts
  friend ModuleEntry;

 public:
  struct Navigation {
    // define all navigation configuration fields
  } navigation;

  struct Telemetry {
    char IP[16];
    char Port[5];
  } telemetry;

//  private:
  void ParseNavigation(char* line);
  void ParseTelemetry(char* line);
  void ParseNone(char* line);


 private:
  explicit Config(char* config_file);
  Config();
  ~Config();
  explicit Config(Config const&);
  Config& operator=(Config const&);
};

}}  // namespace hyped::utils

#endif  // UTILS_CONFIG_HPP_
