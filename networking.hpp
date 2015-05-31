#pragma once

#include <cstdint>
#include "host.hpp"
namespace obiden {
	class Network {
	public:
	    static void SendPacket(uint8_t* packet, const IpAddress* ip_addresses, int num_ip_addresses);
		static int GetNumHosts();
		static void GetIpAddresses(uint32_t* addresses, int num_addresses);
		static uint32_t GetMyIpAddress();
};
}
