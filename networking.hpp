#pragma once

#include <cstdint>
#include "host.hpp"
namespace obiden {
	class Network {
	public:
	    static void SendPacket(uint8_t* packet, const IpAddress* ip_addresses, int num_ip_addresses);
};
}
