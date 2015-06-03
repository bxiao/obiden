#include "host.h"
#include <thread>
#include <iostream>
#include <fstream>
#include <string>

using std::thread;
using std::ifstream;
using std::string;

using namespace obiden;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Wrong number of args." << std::endl;
		return 1;
	}
	
	int self_index = std::stoi(argv[2]);

	vector<HostInfo> hostinfo_vector;
	HostInfo hostinfo;
	auto input = ifstream(argv[1]);
	string ip;
	while (std::getline(input, ip)) {
		int port_start = ip.find(':');
		hostinfo.hostname = ip.substr(0, port_start);
		hostinfo.port = std::stoi(ip.substr(port_start + 1));
		hostinfo_vector.push_back(hostinfo);
	}
	

	Host host(hostinfo_vector.size(), self_index);
	Network network(host, hostinfo_vector);
    
    thread listener_thread(Network::CreateListener, network, 
		hostinfo_vector[self_index].port);

	// create the client thread
	// create timer thread

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
