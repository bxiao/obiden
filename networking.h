#pragma once

#include <cstdint>
#ifdef WIN32
#include <Winsock2.h>
typedef uint32_t in_addr_t;
#else
#include <arpa/inet.h>
#endif

#include <vector>
#include <string>
using std::vector;
using std::string;

namespace obiden {

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

class Network {
    private:
        static vector<HostInfo> host_info_vector;
        static HostInfo client_info;
        static void SendPacketInThread(uint8_t* payload, int payload_size, HostInfo host_info);
    public:
        static void Init(const vector<HostInfo>& host_info_vector, const HostInfo& client_info) {
            Network::host_info_vector = host_info_vector;
            Network::client_info = client_info;
        }

        // Send to hosts (may or may not be single)
        // Create thread per num_ip_addresses
        static void SendPacket(uint8_t* payload, int payload_size, int index) {
            SendPackets(payload, payload_size, vector<int>{index}, false);
        }
        static void SendPackets(uint8_t* payload, int payload_size, const vector<int>& indices,
            bool to_client = false);
        static void CreateListener(int portnum);
};
}
