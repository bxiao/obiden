#pragma once

#include <cstdint>
#ifdef WIN32
#include <Winsock2.h>
typedef uint32_t in_addr_t;
#else
#include <arpa/inet.h>
#endif
#include "host.h"

struct NetworkInfo {
    uint8_t *packet;
    string hostname;
    int port;
    int packetSize;
};

struct HostInfo_t {
    string hostname;
    int port;
};


namespace obiden {
    class Network {
        private:
            int numHosts;
        public:
            // Send to hosts (may or may not be single)
            // Create thread per num_ip_addresses
            static void SendOnePacket(NetInfo_t net_info);
            static void SendPackets(uint8_t* payload, const in_addr_t* ip_addresses, int num_ip_addresses);
            static void CreateListener(int portnum);
    };
}
