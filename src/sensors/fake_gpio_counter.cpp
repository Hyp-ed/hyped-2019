/*
 * Author:
 * Organisation: HYPED
 * Date:
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


namespace hyped {

namespace sensors {

/**
 * Testing Keyence stripe detection: logger, data, start_time, max_time, stripe count (same as data?), missed stripe (bool), acceleration (bool)
 * 
 * Check if missed stripe:
 * compare time with fake_acceleration data- see if accelerating or decelerating
 * switch states of of state machine in consideration of acceleration
 * increase stripe count of missed, log sensor failure
 * 
 * Reach end of run if:
 * max speed based off acceleration/time
 * max time based acceleration/velocity and stripe count
 * max distance based off stripe count
 * --> case: end gracefully and emergency state
 * 
 * 
 */


}}
