#pragma once

#include <cstdint>
#include <vector>
#ifdef WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <memory>

#include "networking.hpp"

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

typedef union {
	uint32_t uint32;
	uint8_t bytes[4];
} IpAddress;

enum Opcode : uint16_t {
	REQUEST_VOTE,
	REQUEST_VOTE_RESPONSE,
	APPEND_ENTRIES,
	EMPTY_APPEND_ENTRIES,
	APPEND_ENTRIES_RESPONSE,
	EMPTY_APPEND_ENTRIES_RESPONSE,
	REQUEST_APPEND_ENTRIES,
	VP_COMBINED_RESPONSE
};

const int SMALL_PACKET_SIZE = 24;
const int LARGE_PACKET_SIZE = 1024;
const int DATA_SIZE = 1000;

struct RequestVotePacket {
	uint16_t size;
	uint16_t opcode;
	uint32_t term;
	uint32_t candidate_index;
	uint32_t last_log_index;
	uint32_t last_log_term;
	uint32_t leftover;
	RequestVotePacket(uint32_t term, uint32_t candidate_index, uint32_t last_log_index,
		uint32_t last_log_term) : size(SMALL_PACKET_SIZE), opcode(Opcode::REQUEST_VOTE),
		term(term), candidate_index(candidate_index), last_log_index(last_log_index),
		last_log_term(last_log_term) { }
	RequestVotePacket& ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		term = htonl(term);
		candidate_index = htonl(candidate_index);
		last_log_index = htonl(last_log_index);
		last_log_term = htonl(last_log_term);
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct RequestVoteResponsePacket {
	uint16_t size;
	uint16_t opcode;
	uint32_t term;
	uint32_t is_vote_granted;
	uint32_t leftover[3];
	RequestVoteResponsePacket(uint32_t term, uint32_t is_vote_granted) : size(SMALL_PACKET_SIZE),
		opcode(Opcode::REQUEST_VOTE_RESPONSE), term(term), is_vote_granted(is_vote_granted) {}
	RequestVoteResponsePacket& ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		term = htonl(term);
		is_vote_granted = htonl(is_vote_granted);
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct EmptyAppendEntriesPacket {
	uint16_t size;
	uint16_t opcode;
	uint32_t term;
	uint32_t previous_log_index;
	uint32_t previous_log_term;
	uint32_t commit_index;
	uint8_t  president_index;
	uint8_t  vp_index;
	uint16_t vp_hosts_bits;
	EmptyAppendEntriesPacket(uint32_t term, uint32_t previous_log_index,
		uint32_t previous_log_term, uint32_t commit_index, uint8_t president_index,
		uint8_t vp_index, uint16_t hosts_bits) : size(SMALL_PACKET_SIZE),
		opcode(Opcode::EMPTY_APPEND_ENTRIES), term(term),
		previous_log_index(previous_log_index), previous_log_term(previous_log_term),
		commit_index(commit_index), president_index(president_index),
		vp_index(vp_index), vp_hosts_bits(hosts_bits) { }
	EmptyAppendEntriesPacket& ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		term = htonl(term);
		previous_log_index = htonl(previous_log_index);
		previous_log_term = htonl(previous_log_term);
		commit_index = htonl(commit_index);
		vp_hosts_bits = htons(vp_hosts_bits);
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct AppendEntriesPacket {
	EmptyAppendEntriesPacket header;
	uint8_t data[DATA_SIZE];
	AppendEntriesPacket(uint32_t term, uint32_t previous_log_index,
		uint32_t previous_log_term, uint32_t commit_index, uint8_t president_index,
		uint8_t vp_index, uint16_t vp_hosts_bits) : header(term, previous_log_index,
		previous_log_term, commit_index, president_index, vp_index, vp_hosts_bits) {
		header.size = LARGE_PACKET_SIZE;
		header.opcode = Opcode::APPEND_ENTRIES; 
	}
	AppendEntriesPacket& ToNetworkOrder() {
		header.ToNetworkOrder();
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct AppendEntriesResponsePacket {
	uint16_t size;
	uint16_t opcode;
	uint32_t term;
	uint32_t success;
	uint32_t sender_index;
	uint32_t leftover[2];
	AppendEntriesResponsePacket(uint32_t term, uint32_t success, uint32_t sender_index) : size(SMALL_PACKET_SIZE),
		opcode(Opcode::APPEND_ENTRIES_RESPONSE), term(term), success(success), sender_index(sender_index) {}
	AppendEntriesResponsePacket & ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		term = htonl(term);
		success = htonl(success);
		sender_index = htonl(sender_index);
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct EmptyAppendEntriesResponsePacket {
	uint16_t size;
	uint16_t opcode;
	uint32_t term;
	uint32_t success;
	uint32_t sender_index;
	uint32_t leftover[2];
	EmptyAppendEntriesResponsePacket(uint32_t term, uint32_t success, uint32_t sender_index) : size(SMALL_PACKET_SIZE),
		opcode(Opcode::EMPTY_APPEND_ENTRIES_RESPONSE), term(term), success(success), sender_index(sender_index) {}
	EmptyAppendEntriesResponsePacket & ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		term = htonl(term);
		success = htonl(success);
		sender_index = htonl(sender_index);
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct RequestAppendEntriesPacket {
	uint16_t size;
	uint16_t opcode;
	uint32_t sender_index;
	uint32_t leftover[4];
	RequestAppendEntriesPacket(uint32_t sender_index) : size(SMALL_PACKET_SIZE), 
		opcode(Opcode::REQUEST_APPEND_ENTRIES), sender_index(sender_index) { }
	RequestAppendEntriesPacket& ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		sender_index = htons(sender_index);
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct VpCombinedResponsePacket {
	uint16_t size;
	uint16_t opcode;
	uint16_t vp_hosts_bits;
	uint16_t vp_hosts_responded_bits;
	uint16_t vp_hosts_success_bits;
	uint16_t vp_hosts_is_empty_bits;
	// since the term is only used to tell if the president has been ousted
	// we just need the highest term from any host
	uint32_t max_term;
	uint16_t leftover[5];
	VpCombinedResponsePacket(uint16_t vp_hosts_bits, uint16_t vp_hosts_responded_bits,
		uint16_t vp_hosts_success_bits, uint32_t max_term) : size(SMALL_PACKET_SIZE),
		opcode(Opcode::VP_COMBINED_RESPONSE), vp_hosts_bits(vp_hosts_bits),
		vp_hosts_responded_bits(vp_hosts_responded_bits), 
		vp_hosts_success_bits(vp_hosts_success_bits),
		max_term(max_term) {}
	VpCombinedResponsePacket& ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		vp_hosts_bits = htons(vp_hosts_bits);
		vp_hosts_responded_bits = htons(vp_hosts_responded_bits);
		vp_hosts_success_bits = htons(vp_hosts_success_bits);
		max_term = htons(max_term);
		return *this;
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

class Host {
	// persistent
	uint32_t term = 0;
	uint8_t voted_for = -1;
	Log log;
	// volitile
	HostState host_state = HostState::FOLLOWER;
	bool is_raft_mode = false;
	uint32_t commit_index = 0;
	uint32_t last_log_index = 0;
	uint32_t self_index = 0;
	uint32_t president_index = 0;
	uint32_t vice_president_index;

	uint32_t num_hosts = 0;
	uint32_t* hosts_next_index = nullptr;
	uint32_t* hosts_match_index = nullptr;
	IpAddress* hosts_ip_address = nullptr;
	int votes_received = 0;

	uint32_t vp_hosts_max_term;
	uint16_t vp_hosts_bits;
	uint16_t vp_hosts_success_bits;
	uint16_t vp_hosts_responded_bits;
	uint16_t vp_hosts_is_empty_bits;

public:
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

	void HandleRequestVote(uint8_t* raw_packet) {
		auto packet = reinterpret_cast<RequestVotePacket*>(raw_packet);
		uint32_t sender_term = ntohl(packet->term);
		uint8_t sender_index = static_cast<uint8_t>(ntohl(packet->candidate_index) & 0xFF);
		uint32_t sender_log_index = ntohl(packet->last_log_index);
		uint32_t sender_log_term = ntohl(packet->last_log_term);

		if (sender_term >= term) {
			uint32_t vote = (voted_for == -1 || voted_for == sender_index) &&
				sender_log_term >= term && sender_log_index >= last_log_index;
			RequestVoteResponsePacket response(term, vote);
			Network::SendPacket(response.ToNetworkOrder().ToBytes(), &hosts_ip_address[sender_index], 1);
			term = sender_term;
			if (sender_term > term) {
				host_state = HostState::FOLLOWER;
			}
		}
		else {
			RequestVoteResponsePacket response(term, 0);
			Network::SendPacket(response.ToNetworkOrder().ToBytes(), &hosts_ip_address[sender_index], 1);
		}
	}

	void HandleRequestVoteResponse(uint8_t* raw_packet) {
		auto packet = reinterpret_cast<RequestVoteResponsePacket*>(raw_packet);
		uint32_t sender_term = ntohl(packet->term);
		uint32_t sender_is_vote_granted = ntohl(packet->is_vote_granted);
		if (sender_term > term) {
			term = sender_term;
			host_state = HostState::FOLLOWER;
			return;
		}
		if (sender_is_vote_granted) {
			++votes_received;
		}
		if (votes_received > num_hosts / 2) {
			host_state = HostState::PRESIDENT;
			for (int i = 0; i < num_hosts; ++i) {
				hosts_next_index[i] = last_log_index + 1;
				hosts_match_index[i] = 0;
			}
			EmptyAppendEntriesPacket packet(term, last_log_index, log[last_log_index].term,
				commit_index, self_index, -1, 0);
			Network::SendPacket(packet.ToNetworkOrder().ToBytes(), hosts_ip_address, num_hosts);
		}
	}

	void HandleAppendEntries(uint8_t* raw_packet, bool is_empty) {
		host_state = HostState::FOLLOWER;
		auto packet = reinterpret_cast<AppendEntriesPacket*>(raw_packet);
		uint8_t  sender_vp_index = packet->header.vp_index;
		static uint8_t original_packet[LARGE_PACKET_SIZE];
		if (sender_vp_index == self_index) {
			memcpy_s(original_packet, LARGE_PACKET_SIZE, raw_packet, is_empty ? SMALL_PACKET_SIZE : LARGE_PACKET_SIZE);
		}

		uint32_t sender_term = ntohl(packet->header.term);
		uint32_t sender_previous_log_index = ntohl(packet->header.previous_log_index);
		uint32_t sender_log_index = sender_previous_log_index + 1;
		uint32_t sender_previous_log_term = ntohl(packet->header.previous_log_term);
		uint32_t sender_commit_index = ntohl(packet->header.previous_log_term);
		uint8_t  sender_president_index = packet->header.president_index;
		// vice president index above
		uint16_t sender_vp_hosts_bits = ntohs(packet->header.vp_hosts_bits);

		if (sender_term < term || 
			    log[sender_previous_log_index].term != sender_previous_log_term) {
			AppendEntriesResponsePacket response(term, 0, self_index);
			Network::SendPacket(response.ToNetworkOrder().ToBytes(), &hosts_ip_address[sender_president_index], 1);
			return;
		}

		if (!is_empty) {
			auto log_entry = reinterpret_cast<LogEntry*>(packet->data)->ToHostOrder();
			if (log.size() > sender_log_index && log[sender_log_index].term != log_entry.term) {
				log.resize(sender_log_index);
			}

			log.push_back(log_entry);

			if (sender_commit_index > commit_index) {
				auto last = log.size() - 1;
				commit_index = sender_commit_index > last ? last : sender_commit_index;
			}
		}

		term = sender_term;

		AppendEntriesResponsePacket response(term, 1, self_index);
		Network::SendPacket(response.ToNetworkOrder().ToBytes(), &hosts_ip_address[sender_president_index], 1);

		if (vice_president_index == self_index) {
			host_state = HostState::VICE_PRESIDENT;
			vp_hosts_bits = sender_vp_hosts_bits;
			vp_hosts_responded_bits = 0;
			vp_hosts_success_bits = 0;
			vp_hosts_is_empty_bits = 0;
			vp_hosts_max_term = 0;
			std::vector<IpAddress> host_addresses;
			for (int i = 0; i < sizeof(sender_vp_hosts_bits) * 8; ++i) {
				uint16_t mask = 1 << i;
				if (sender_vp_hosts_bits & mask) {
					host_addresses.push_back(hosts_ip_address[i]);
				}
			}
		    Network::SendPacket(original_packet, host_addresses.data(), host_addresses.size());
		}
	}

	void HandleAppendEntriesResponse(uint8_t* raw_packet, bool is_empty) {
		auto packet = reinterpret_cast<AppendEntriesResponsePacket*>(raw_packet);
		uint32_t sender_term = ntohl(packet->term);
		uint32_t sender_success = ntohl(packet->success);
		uint32_t sender_index = ntohl(packet->sender_index);

		if (host_state == HostState::FOLLOWER) {
			return;
		}
		
		if (host_state == HostState::VICE_PRESIDENT) {
			VpHandleAppendEntriesResponse(sender_term, sender_success, sender_index, is_empty);
			return;
		}

		if (host_state == HostState::PRESIDENT) {
			if (term < sender_term) {
				host_state == HostState::FOLLOWER;
				return;
			} else {
				PresidentHandleAppendEntriesResponse(sender_success, sender_index, is_empty);
			}
		}
	}

	void VpHandleAppendEntriesResponse(uint32_t follower_term, bool follower_success,
		uint32_t follower_index, bool follower_is_empty) {
		if (follower_term > vp_hosts_max_term) {
			vp_hosts_max_term = follower_term;
		}
		uint16_t mask = 1 << follower_index;
		vp_hosts_responded_bits |= mask;
		if (follower_success) {
			vp_hosts_success_bits |= mask;
		}
		else {
			vp_hosts_success_bits &= ~mask;
		}
		if (follower_is_empty) {
			vp_hosts_is_empty_bits |= mask;
		}
		else {
			vp_hosts_is_empty_bits &= ~mask;
		}

		if (vp_hosts_bits == vp_hosts_responded_bits) {
			VpCombinedResponsePacket packet(vp_hosts_bits, vp_hosts_responded_bits,
				vp_hosts_success_bits, vp_hosts_max_term);
			Network::SendPacket(packet.ToNetworkOrder().ToBytes(), &hosts_ip_address[president_index], 1);
		}
	}

	void PresidentHandleAppendEntriesResponse(bool follower_success, uint32_t follower_index, bool is_empty) {
		if (follower_success) {
			if (!is_empty) {
				++hosts_next_index[follower_index];
				hosts_match_index[follower_index] = hosts_next_index[follower_index];
			}
		} else {
			--hosts_next_index[follower_index];
			// signal condition variable
			
		}
	}

	void HandleRequestAppendEntries(uint8_t* raw_packet) {

		if (host_state == HostState::PRESIDENT) {
			vice_president_index = -1;
			auto packet = reinterpret_cast<RequestAppendEntriesPacket*>(raw_packet);
			uint32_t sender_index = ntohl(packet->sender_index);

			EmptyAppendEntriesPacket response(term, last_log_index, log[last_log_index].term,
				commit_index, self_index, -1, 0);

			Network::SendPacket(response.ToNetworkOrder().ToBytes(), &hosts_ip_address[sender_index], 1);
		}
	}


	void HandleVpCombinedResponse(uint8_t* raw_packet) {
		auto packet = reinterpret_cast<VpCombinedResponsePacket*>(raw_packet);
		uint16_t sender_vp_hosts_bits = ntohs(packet->vp_hosts_bits);
		uint16_t sender_vp_hosts_responded_bits = ntohs(packet->vp_hosts_responded_bits);
		uint16_t sender_vp_hosts_success_bits = ntohs(packet->vp_hosts_success_bits);
		uint16_t sender_vp_hosts_is_empty_bits = ntohs(packet->vp_hosts_is_empty_bits);
		uint32_t sender_max_term = ntohl(packet->max_term);

		if (term < sender_max_term) {
			term = sender_max_term;
			host_state = HostState::FOLLOWER;
			return;
		}
		else {
			for (int i = 0; i < sizeof(uint16_t); ++i) {
				uint16_t mask = 1 << i;
				if (sender_vp_hosts_responded_bits & mask) {
					bool success = (sender_vp_hosts_success_bits & mask) != 0;
					bool is_empty = (sender_vp_hosts_is_empty_bits & mask) != 0;
					PresidentHandleAppendEntriesResponse(success, i, is_empty);
				}
			}
		}
	}

	void RoutePacket(uint8_t* packet) {
		auto opcode = ToUint16(packet + 2);
		switch (opcode) {
		case REQUEST_VOTE:
			HandleRequestVote(packet);
			return;
		case REQUEST_VOTE_RESPONSE:
			HandleRequestVoteResponse(packet);
			return;
		case APPEND_ENTRIES:
			HandleAppendEntries(packet, false);
			return;
		case EMPTY_APPEND_ENTRIES:
			HandleAppendEntries(packet, true);
			return;
		case APPEND_ENTRIES_RESPONSE:
			HandleAppendEntriesResponse(packet, false);
			return;
		case EMPTY_APPEND_ENTRIES_RESPONSE:
			HandleAppendEntriesResponse(packet, true);
			return;
		case REQUEST_APPEND_ENTRIES:
			HandleRequestAppendEntries(packet);
			return;
		case VP_COMBINED_RESPONSE:
			HandleVpCombinedResponse(packet);
			return;
		default:
			// ignore packet
			return;
		}
	}

};

}
