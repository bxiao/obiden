#pragma once

#include <cstdint>
#ifdef WIN32
#include <Winsock2.h>
typedef uint32_t in_addr_t;
#else
#include <arpa/inet.h>
#endif
#include "host.h"

#include <vector>
#include <string>
using std::vector;
using std::string;

struct NetworkInfo {
    uint8_t *packet;
    string hostname;
    int port;
    int payloadSize;
};

struct HostInfo {
    string hostname;
    int port;
};


namespace obiden {
    class Network {
        private:
			vector<HostInfo> host_info_vector;
			HostInfo client_info;
			static void SendOnePacket(uint8_t* payload, int payload_size, HostInfo host_info);
			Host host;
        public:
			Network(Host host, vector<HostInfo> host_info_vector, HostInfo client_info) : host(host), 
				host_info_vector(host_info_vector), client_info(client_info) {}

            // Send to hosts (may or may not be single)
            // Create thread per num_ip_addresses
            
            void SendPackets(uint8_t* payload, int payload_size, const vector<int>& indices,
				bool to_client = false);
            static void CreateListener(Network network, int portnum);
    };
}
