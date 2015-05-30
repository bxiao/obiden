#pragma once

#include <cstdint>
#include <vector>
#ifdef WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "networking.hpp"

#pragma pack(1)

namespace obiden {

	enum class HostState {
		FOLLOWER,
		CANDIDATE,
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
		void ToNetworkOrder() {
			size = htons(size);
			opcode = htons(opcode);
			term = htonl(term);
			candidate_index = htonl(candidate_index);
			last_log_index = htonl(last_log_index);
			last_log_term = htonl(last_log_term);
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
		void ToNetworkOrder() {
			size = htons(size);
			opcode = htons(opcode);
			term = htonl(term);
			is_vote_granted = htonl(is_vote_granted);
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
		uint8_t  president_id;
		uint8_t  vice_president_id;
		uint16_t hosts_bit_field;
		EmptyAppendEntriesPacket(uint32_t term, uint32_t previous_log_index,
			uint32_t previous_log_term, uint32_t commit_index, uint8_t president_id,
			uint8_t vice_president_id, uint16_t hosts_bit_field) : size(SMALL_PACKET_SIZE),
			opcode(Opcode::EMPTY_APPEND_ENTRIES), term(term),
			previous_log_index(previous_log_index), previous_log_term(previous_log_term),
			commit_index(commit_index), president_id(president_id),
			vice_president_id(vice_president_id), hosts_bit_field(hosts_bit_field) { }
		void ToNetworkOrder() {
			size = htons(size);
			opcode = htons(opcode);
			term = htonl(term);
			previous_log_index = htonl(previous_log_index);
			previous_log_term = htonl(previous_log_term);
			commit_index = htonl(commit_index);
			hosts_bit_field = htons(hosts_bit_field);
		}
		uint8_t* ToBytes() {
			return reinterpret_cast<uint8_t*>(this);
		}
	};

	struct AppendEntriesPacket {
		EmptyAppendEntriesPacket header;
		uint8_t data[DATA_SIZE];
		AppendEntriesPacket(uint32_t term, uint32_t previous_log_index,
			uint32_t previous_log_term, uint32_t commit_index, uint8_t president_id,
			uint8_t vice_president_id, uint16_t hosts_bit_field) : header(term, previous_log_index,
			previous_log_term, commit_index, president_id, vice_president_id, hosts_bit_field) {
			header.size = LARGE_PACKET_SIZE;
			header.opcode = Opcode::APPEND_ENTRIES; 
		}
		void ToNetworkOrder() {
			header.ToNetworkOrder();
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
	uint32_t leftover[3];
	AppendEntriesResponsePacket(uint32_t term, uint32_t success) : size(SMALL_PACKET_SIZE),
		opcode(Opcode::APPEND_ENTRIES_RESPONSE), term(term), success(success) {}
	void ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		term = htonl(term);
		success = htonl(success);
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct RequestAppendEntries {
	uint16_t size;
	uint16_t opcode;
	uint32_t leftover[5];
	RequestAppendEntries() : size(SMALL_PACKET_SIZE), opcode(Opcode::REQUEST_APPEND_ENTRIES) {}
	void ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
	}
	uint8_t* ToBytes() {
		return reinterpret_cast<uint8_t*>(this);
	}
};

struct VpCombinedResponse {
	uint16_t size;
	uint16_t opcode;
	uint16_t hosts_bit_field;
	uint16_t responded_bit_field;
	uint16_t success_bit_field;
	// since the term is only used to tell if the president has been ousted
	// we just need the highest term from any host
	uint32_t max_term;
	uint16_t leftover[5];
	VpCombinedResponse(uint16_t hosts_bit_field, uint16_t responded_bit_field,
		uint16_t success_bit_field, uint32_t max_term) : size(SMALL_PACKET_SIZE),
		opcode(Opcode::VP_COMBINED_RESPONSE), hosts_bit_field(hosts_bit_field),
		responded_bit_field(responded_bit_field), success_bit_field(success_bit_field),
		max_term(max_term) {}
	void ToNetworkOrder() {
		size = htons(size);
		opcode = htons(opcode);
		hosts_bit_field = htons(hosts_bit_field);
		responded_bit_field = htons(responded_bit_field);
		success_bit_field = htons(success_bit_field);
		max_term = htons(max_term);
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
		uint32_t candidate_term = ntohl(packet->term);
		uint8_t candidate_index = static_cast<uint8_t>(ntohl(packet->candidate_index) & 0xFF);
		uint32_t candidate_log_index = ntohl(packet->last_log_index);
		uint32_t candidate_log_term = ntohl(packet->last_log_term);

		if (candidate_term >= term) {
			uint32_t vote = (voted_for == -1 || voted_for == candidate_index) &&
				candidate_log_term >= term && candidate_log_index >= last_log_index;
			RequestVoteResponsePacket response(term, vote);
			response.ToNetworkOrder();
			Network::SendPacket(response.ToBytes(), &hosts_ip_address[candidate_index], 1);
			term = candidate_term;
			if (candidate_term > term) {
				host_state = HostState::FOLLOWER;
			}
		}
		else {
			RequestVoteResponsePacket response(term, 0);
			response.ToNetworkOrder();
			Network::SendPacket(response.ToBytes(), &hosts_ip_address[candidate_index], 1);
		}
	}

	void HandleRequestVoteResponse(uint8_t* raw_packet) {
		auto packet = reinterpret_cast<RequestVoteResponsePacket*>(raw_packet);
		uint32_t voter_term = ntohl(packet->term);
		uint32_t is_vote_granted = ntohl(packet->is_vote_granted);
		if (voter_term > term) {
			term = voter_term;
			host_state = HostState::FOLLOWER;
			return;
		}
		if (is_vote_granted) {
			++votes_received;
		}
		if (votes_received > num_hosts / 2) {
			host_state = HostState::PRESIDENT;
			EmptyAppendEntriesPacket packet(term, last_log_index, log[last_log_index].term,
				commit_index, self_index, -1, 0);
			packet.ToNetworkOrder();
			Network::SendPacket(packet.ToBytes(), hosts_ip_address, num_hosts);
		}
	}

	void HandleAppendEntries(uint8_t* raw_packet) {
		host_state = HostState::FOLLOWER;
		auto packet = reinterpret_cast<AppendEntriesPacket*>(raw_packet);
		uint32_t president_term = ntohl(packet->header.term);
		uint32_t president_previous_log_index = ntohl(packet->header.previous_log_index);
		uint32_t president_log_index = president_previous_log_index + 1;
		uint32_t president_previous_log_term = ntohl(packet->header.previous_log_term);
		uint32_t president_commit_index = ntohl(packet->header.previous_log_term);
		uint8_t  president_president_id = packet->header.president_id;
		uint8_t  president_vice_president_id = packet->header.vice_president_id;
		uint16_t president_hosts_bit_field = ntohs(packet->header.hosts_bit_field);

		if (president_term < term || 
			    log[president_previous_log_index].term != president_previous_log_term) {
			AppendEntriesResponsePacket response(term, 0);
			response.ToNetworkOrder();
			Network::SendPacket(response.ToBytes(), &hosts_ip_address[president_president_id], 1);
			return;
		}

		auto log_entry = reinterpret_cast<LogEntry*>(packet->data)->ToHostOrder();
		if (log.size() > president_log_index && log[president_log_index].term != log_entry.term) {
			log.resize(president_log_index);
		}

		log.push_back(log_entry);

		if (president_commit_index > commit_index) {
			auto last = log.size() - 1;
			commit_index = president_commit_index > last ? last : president_commit_index;
		}

		term = president_term;

		AppendEntriesResponsePacket response(term, 1);
		response.ToNetworkOrder();
		Network::SendPacket(response.ToBytes(), &hosts_ip_address[president_president_id], 1);
	}

	void HandleEmptyAppendEntries(uint8_t* raw_packet) {

	}

	void HandleAppendEntriesResponse(uint8_t* raw_packet) {

	}

	void HandleRequestAppendEntries(uint8_t* raw_packet) {

	}


	void HandleVpCombinedResponse(uint8_t* raw_packet) {
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
			HandleAppendEntries(packet);
			return;
		case EMPTY_APPEND_ENTRIES:
			HandleEmptyAppendEntries(packet);
			return;
		case APPEND_ENTRIES_RESPONSE:
			HandleAppendEntriesResponse(packet);
			return;
		case REQUEST_APPEND_ENTRIES:
			HandleRequestAppendEntries(packet);
			return;
		case VP_COMBINED_RESPONSE:
			HandleVpCombinedResponse(packet);
			return;
		}


	}

};

}
