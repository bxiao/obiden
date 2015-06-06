#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

using std::cout;
using std::endl;
using std::string;
using std::thread;
using std::mutex;
using std::condition_variable;
using std::unique_lock;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

mutex timer_mutex;
condition_variable timer_cv;

mutex timeout_event_mutex;
condition_variable timeout_event_cv;

auto rand_timeout = std::bind(std::uniform_int_distribution<int>(150, 300), 
    std::default_random_engine(high_resolution_clock::now().time_since_epoch().count()));

enum WaitTime {
    INFINITE = -2,
    ELECTION_RANDOM = -1,
    HEARTBEAT = 100,
    VICE_PRESIDENT = 50
};
auto wait_time = ELECTION_RANDOM;


void Timer() {
    while (true) {
        auto wait_ms = milliseconds(0);
        if (wait_time == INFINITE) {
            cout << "Timer is inactive"  << endl;
        }
        else if (wait_time == ELECTION_RANDOM) {
            wait_ms = milliseconds(rand_timeout());
            cout << "Timeout set for " << wait_ms.count() << " ms" << endl;
        }
        else {
            wait_ms = milliseconds(wait_time);
            cout << "Timeout set for " << wait_ms.count() << " ms" << endl;
        }
        unique_lock<mutex> lock(timer_mutex);
        if (wait_time == INFINITE) {
            timer_cv.wait(lock);
            cout << "Timer activated" << endl;
        }
        else {
            auto status = timer_cv.wait_for(lock, milliseconds(rand_timeout()));
            if (status == std::cv_status::timeout) {
                cout << "Timer timed out" << endl;
                unique_lock<mutex> event_lock(timeout_event_mutex);
                timeout_event_cv.notify_one();
            }
            else {
                cout << "Timer reset" << endl;
            }
        }
    }
}

void Handler() {
    while (true) {
        std::this_thread::sleep_for(milliseconds(180));
        cout << "Sending heartbeat" << endl;
        std::unique_lock<mutex>(timer_mutex);
        timer_cv.notify_one();
    }
}

int main() {

    auto handler_thread = thread(Handler);
    auto timer_thread = thread(Timer);

    while (true) {
        cout << "Waiting for timeout" << endl;
        auto lock = unique_lock<mutex>(timeout_event_mutex);
        timeout_event_cv.wait(lock);
        cout << "Got timeout, if this were not a test I would move to candidate state" << endl;
    }

    return 0;
}
