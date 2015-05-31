#pragma once

#include <cstdint>
#ifdef WIN32
#include <Winsock2.h>
typedef uint32_t in_addr_t;
#else
#include <arpa/inet.h>
#endif

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
