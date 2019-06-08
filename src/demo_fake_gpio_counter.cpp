#include "utils/concurrent/thread.hpp"
#include "sensors/fake_gpio_counter.hpp"
#include "utils/logger.hpp"
#include "utils/timer.hpp"
#include "utils/system.hpp"
#include "data/data.hpp"

using hyped::utils::concurrent::Thread;
using hyped::sensors::FakeGpioCounter;
using hyped::utils::Logger;
using hyped::utils::Timer;
using hyped::data::StripeCounter;
using hyped::data::Data;
using hyped::data::State;

uint8_t kStripeNum = 30;

int main(int argc, char* argv[]) {
    hyped::utils::System::parseArgs(argc, argv);
    Logger log(true, 0);
    FakeGpioCounter fake_gpio(log, false, "data/in/gpio_counter_normal_run.txt");
    
    Data& data = Data::getInstance();
    uint32_t stripe_count = 0;

    auto state = data.getStateMachineData();
    state.current_state = State::kAccelerating;
    data.setStateMachineData(state);

    while (stripe_count < kStripeNum){
        StripeCounter stripe_counted = fake_gpio.getStripeCounter();
        stripe_count = stripe_counted.count.value;
        log.DBG("FakeGpioCounter", "Stripe count = %u", stripe_count);
        Thread::sleep(50);
    }
}