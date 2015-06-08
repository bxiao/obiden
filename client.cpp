#pragma once

using namespace std;

// CLIENT ONLY
int presidentIndex = 0;


void createListener(int portnum)
{
    int sk = 0;
    struct sockaddr_in local;
    int len = sizeof(local);

    // Create listener socket and process packets here
    if((sk = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Socket Call");
        exit(-1);
    }
    //set up the socket
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(portnum);

    //bind the name (address) to a port
    if(bind(sk,(struct sockaddr*)&local, sizeof(local)) < 0){
        perror("bind call");
        exit(-1);
    }

    int messageLength = 0;
    struct sockaddr_in remote;
    int rlen = sizeof(remote);

    if(getsockname(sk,(struct sockaddr*)&local,&len) < 0){
        perror("getsockname call");
        exit(-1);
    }
    printf("socket has port %d \n", ntohs(local.sin_port));

    // Wait for connection
    while (true) {
        // Wait for packets, and parse them as they come in
        // Can be 24 bytes or 1024, messageLength will be the determinant of what packet type it is
        auto packet = new uint8_t[LARGE_PACKET_SIZE];
        messageLength = recvfrom(sk, reinterpret_cast<char*>(packet), LARGE_PACKET_SIZE, 0,
            (struct sockaddr*) &remote, &rlen);
        std::cout << "mesglen: " << messageLength << "\npayload: " << packet << '\n';

        if (messageLength == sizeof(AppendEntriesPacket)) {
            presidentIndex = (AppendEntriesPacket)packet.header.president_index;
            sendPacketInThread(payload, payload_size, host_info_vector[presidentIndex]);
        }
        if (messageLength == sizeof(__packet__)) {
            time_t timestamp;
            struct tm* tm_info = localtime(&timestamp);

            strftime(buffer, 25, "%H:%M:%S", tm_info);
            cout "time: " << buffer << " go back to listen\n";
        }
    }
}

void sendPackets(uint8_t *payload, int payload_size, const vector<int>& indices, bool to_client)
{
    for (auto index: indices)
    {
        auto host_thread = thread(SendPacketInThread, payload, payload_size, host_info_vector[index]);
    }
    if (to_client) {
        auto client_thread = thread(SendPacketInThread, payload, payload_size, client_info);
    }
}

void sendPacketInThread(uint8_t *payload, int payload_size, HostInfo host_info)
{
    int sk = 0;
    struct sockaddr_in remote;
    struct hostent *hp;

    auto packet = new char[payload_size];
    memcpy(packet, payload, payload_size);

    if ((sk = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket call");
        exit(-1);
    }

    remote.sin_family = AF_INET;
    hp = gethostbyname(host_info.hostname.c_str());
    memcpy(&remote.sin_addr, hp->h_addr, hp->h_length);
    remote.sin_port = htons(host_info.port);

    sendto(sk, packet, payload_size, 0, reinterpret_cast<sockaddr*>(&remote), sizeof(remote));

    delete[] packet;
}

// Send an initial message and then wait to hear back from the leader.
    // Why send initial message?
// Then send another message just as soon as you hear back.

// Rather than make the hosts keep track of performance the client can just record the timestamp of each commit message received from the president.
// Dump to standard out.

int main(int argc, char* argv)
{
    vector<HostInfo> hostinfo_vector;
    HostInfo hostinfo;
    auto input = ifstream(argv[1]);
    string ip;
    while (std::getline(input, ip)) {
        int port_start = ip.find(':');
        hostinfo.hostname = ip.substr(0, port_start);
        hostinfo.port = std::stoi(ip.substr(port_start + 1));
        hostinfo_vector.push_back(hostinfo);
    }

    auto client_host = hostinfo_vector.back();
    hostinfo_vector.pop_back();

    auto listener_thread = thread(createListener, hostinfo_vector[self_index].port);

    return 0;
}
