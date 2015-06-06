#include "timer.h"
#include "host.h"
namespace obiden {
void Timer::Run() {
    wait_time = ELECTION_RANDOM;
    auto rand_timeout = std::bind(std::uniform_int_distribution<int>(150, 300),
        std::default_random_engine(high_resolution_clock::now().time_since_epoch().count()));
    while (true) {
        unique_lock<mutex> lock(timer_mutex);
        auto wait_ms = wait_time == ELECTION_RANDOM ? milliseconds(rand_timeout()) :
            milliseconds(wait_time);
        auto status = timer_cv.wait_for(lock, wait_ms);
        if (status == std::cv_status::timeout) {
            unique_lock<mutex> event_lock(Host::event_mutex);
            Host::event_cv.notify_one();
        }
    }
}

void Timer::Reset() {
    unique_lock<mutex> lock(timer_mutex);
    timer_cv.notify_one();
}
void Timer::Change(WaitTime wait_time_in) {
    unique_lock<mutex> lock(timer_mutex);
    wait_time = wait_time_in;
    timer_cv.notify_one();
}
}