#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

using std::thread;
using std::mutex;
using std::condition_variable;
using std::unique_lock;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace obiden {

class Timer {
public:
    static mutex timer_mutex;
    static condition_variable timer_cv;

    enum WaitTime {
        ELECTION_RANDOM = -1,
        HEARTBEAT = 100,
        VICE_PRESIDENT = 50
    };

    static WaitTime wait_time;

    static void Run();
    static void Reset();
    static void Change(WaitTime wait_time);
};

}