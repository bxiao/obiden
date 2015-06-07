#include "timer.h"

namespace obiden {
void Timer::Run() {
    wait_time = ELECTION_RANDOM;
    auto rand_timeout = std::bind(std::uniform_int_distribution<int>(150, 300),
        std::default_random_engine(high_resolution_clock::now().time_since_epoch().count()));
	while (true) 
		if (Host::host_state == HostState::VICE_PRESIDENT) {
			vp_start_time = high_resolution_clock::now().time_since_epoch();
		}
        unique_lock<mutex> lock(timer_mutex);
        auto wait_ms = wait_time == ELECTION_RANDOM ? milliseconds(rand_timeout()) :
            milliseconds(wait_time);
        auto status = timer_cv.wait_for(lock, wait_ms);
		if (Host::host_state == HostState::VICE_PRESIDENT) {
			vp_elapsed_time = high_resolution_clock::now().time_since_epoch() - vp_start_time;
			wait_ms 
		}
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
void Timer::ChangeState(HostState host_state) {
    unique_lock<mutex> lock(timer_mutex);
	if (host_state == HostState::PRESIDENT) {
		wait_time = WaitTime::HEARTBEAT;
	}
	else if (host_state == HostState::CANDIDATE || host_state == HostState::FOLLOWER) {
		wait_time = WaitTime::ELECTION_RANDOM;
	}
	else {

		wait_time = WaitTime::VICE_PRESIDENT;
	}
    timer_cv.notify_one();
}
}