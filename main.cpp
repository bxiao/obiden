#include "host.h"

using namespace obiden;

int main(int argc, char* argv[]) {
    Host host;

    // Create listener thread
    
    std::thread t1(Network::CreateListener, self.portnum);

    while (true) {
        switch (host.host_state) {
        case HostState::PRESIDENT:
            host.PresidentState();
            return;
        case HostState::VICE_PRESIDENT:
            host.VicePresidentState();
            return;
        case HostState::CANDIDATE:
            host.CandidateState();
            return;
        case HostState::FOLLOWER:
            host.FollowerState();
            return;
        default:
            // be super sad, something went bad.
        }

    }

    return 0;
}
