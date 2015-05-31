#pragma once

#include <cstdint>
#ifdef WIN32
#include <Winsock2.h>
typedef uint32_t in_addr_t;
#else
#include <arpa/inet.h>
#endif

#include "host.h"
namespace obiden {
	class Network {
	public:
	    static void SendPacket(uint8_t* packet, const in_addr_t* ip_addresses, int num_ip_addresses);
		static int GetNumHosts();
		static void GetIpAddresses(uint32_t* addresses, int num_addresses);
		static uint32_t GetMyIpAddress();
};
}
