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

constexpr uint32_t kStripeNum = 1000;          // depending on configuration of run

int main(int argc, char* argv[]) {
    hyped::utils::System::parseArgs(argc, argv);
    Logger log(true, 0);
    GPIO pin(69, hyped::utils::io::gpio::kIn);          // initialise gpio
    
    uint32_t stripe_count = 0;
    Timer timer;      // get timer started
    timer.reset();
    timer.start();
    Thread::sleep(500);
    uint64_t start_time = timer.getTimeMicros();
    log.INFO("TEST-KEYENCE", "Start time: %f", start_time);
    uint8_t val = pin.wait();

    while (stripe_count < kStripeNum){
        log.DBG("KEYENCE-TEST","Waiting");
        val = pin.wait();
        if (val == 1)                   // if hit stripe --> gpio to high
        {
            // log.DBG("KEYENCE-TEST","Hit stripe at: %d micros",timer.getTimeMicros());
            stripe_count++;
            log.DBG("KEYENCE-TEST","Stripe Count: %d",stripe_count);
        }
    }

    timer.stop();
    log.INFO("KEYENCE-TEST", "Final stripe count = %d. Final timestamp = %d", stripe_count, timer.getMicros());
}