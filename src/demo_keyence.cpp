#include "utils/concurrent/thread.hpp"
#include "utils/io/gpio.hpp"
#include "sensors/gpio_counter.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"

using hyped::utils::concurrent::Thread;
using hyped::utils::io::GPIO;
using hyped::sensors::GpioCounter;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::data::StripeCounter;

constexpr uint32_t kStripeNum = 500;          // depending on configuration of run

int main(int argc, char* argv[]) {
    hyped::utils::System::parseArgs(argc, argv);
    Logger log(true, 1);
    GPIO pin(66, hyped::utils::io::gpio::kOut);
    GpioCounter keyence(69);
    
    Timer timer;      // get timer started
    timer.reset();
    timer.start();
    Thread::sleep(500);
    uint64_t start_time = timer.getTimeMicros();
    log.INFO("TEST-KEYENCE", "Start time: %f", start_time);

    // keyence.start();
    keyence.run();
    // keyence.sleep(50);
    StripeCounter stripe_data = keyence.getStripeCounter();
    uint32_t stripe_count = 0;
    while (stripe_count < kStripeNum) {
      stripe_data = keyence.getStripeCounter();
      if (stripe_data.count.value > stripe_count) {
        stripe_count = stripe_data.count.value;
        log.DBG("KEYENCE-TEST","Stripe Count: %d",stripe_count);
      }
      Thread::sleep(50);      // remove if want to see output quickly
    }
    timer.stop();
    log.INFO("KEYENCE-TEST", "Final stripe count = %d. Final timestamp = %d", stripe_count, timer.getMicros());
}