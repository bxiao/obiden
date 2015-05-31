#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "packets.h"
#include "networking.hpp"

using std::mutex;
using std::condition_variable;

#pragma pack(1)

namespace obiden {

enum class HostState {
	FOLLOWER,
	CANDIDATE,
	VICE_PRESIDENT,
	PRESIDENT
};

struct LogEntry {
	uint32_t term;
	uint32_t timestamp;
	uint32_t data;
	LogEntry(uint32_t term, uint32_t timestamp, uint32_t data): term(term), 
		timestamp(timestamp), data(data) { }
	LogEntry& ToNetworkOrder() {
		term = htonl(term);
		timestamp = htonl(timestamp);
		data = htonl(data);
		return *this;
	}
	LogEntry& ToHostOrder() {
		term = ntohl(term);
		timestamp = ntohl(timestamp);
		data = ntohl(data);
	}
};

typedef std::vector<LogEntry> Log;

class Host {
	// persistent
	uint32_t term = 0;
	uint8_t voted_for = -1;
	Log log;
	// volitile
	bool is_raft_mode = false;
	uint32_t commit_index = 0;
	uint32_t last_log_index = 0;
	uint32_t self_index = 0;
	uint32_t president_index = 0;
	uint32_t vice_president_index;

	uint32_t num_hosts = 0;
	uint32_t* hosts_next_index = nullptr;
	uint32_t* hosts_match_index = nullptr;
	in_addr_t* hosts_ip_address = nullptr;
	int votes_received = 0;

	uint32_t vp_hosts_max_term;
	uint16_t vp_hosts_bits;
	uint16_t vp_hosts_success_bits;
	uint16_t vp_hosts_responded_bits;
	uint16_t vp_hosts_is_empty_bits;

public:
	Host() {
		num_hosts = Network::GetNumHosts();
		hosts_ip_address = new in_addr_t[num_hosts];
		Network::GetIpAddresses(hosts_ip_address, num_hosts);
		auto my_ip_address = Network::GetMyIpAddress();
		for (int i = 0; i < num_hosts; ++i) {
			if (my_ip_address == hosts_ip_address[i]) {
				self_index = i;
			}
		}
	}

	HostState host_state = HostState::FOLLOWER;

	mutex election_timeout_mutex;
	condition_variable election_timeout_cv;

	mutex 

	void PresidentState();
	void VicePresidentState();
	void CandidateState();
	void FollowerState();

	void ElectionTimer();

	static uint16_t ToUint16(uint8_t* data) {
		return (data[0] << 8) | data[1];
	}
	static uint32_t ToUint32(uint8_t* data) {
		return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	}
	static void ToBytes(uint16_t value, uint8_t* data) {
		data[0] = static_cast<uint8_t>(value >> 8);
		data[1] = static_cast<uint8_t>(value & 0xFF);
	}
	static void ToBytes(uint32_t value, uint8_t* data) {
		data[0] = static_cast<uint8_t>(value >> 24);
		data[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
		data[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
		data[3] = static_cast<uint8_t>((value)& 0xFF);
	}

	void HandleRequestVote(uint8_t* raw_packet);
	void HandleRequestVoteResponse(uint8_t* raw_packet);
	void HandleAppendEntries(uint8_t* raw_packet, bool is_empty);
	void HandleAppendEntriesResponse(uint8_t* raw_packet, bool is_empty);
	void VpHandleAppendEntriesResponse(uint32_t follower_term, bool follower_success,
		uint32_t follower_index, bool follower_is_empty);
	void PresidentHandleAppendEntriesResponse(bool follower_success, uint32_t follower_index, bool is_empty);
	void HandleRequestAppendEntries(uint8_t* raw_packet);
	void HandleVpCombinedResponse(uint8_t* raw_packet);
	void RoutePacket(uint8_t* packet);
};

}
