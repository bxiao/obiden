#pragma once
#include "networks.h"

/*
 * SetupNetwork()
 * Opens the networking configuration file and sets up the multicast socket communication between
 * the nodes in the cluster.
 *
 * There will be a fixed number of hosts that will be part of the cluster, and the file layout will
 * be as such:
 *
 * <ip_address_host_1> <port_1>
 * [...]
 * <ip_address_host_n> <port_n>
 *
 */

void Network::CreateListener(int portnum)
{   
    int sk = 0;
    struct sockaddr_in local;
    uint32_t len = sizeof(local);

    // Create listener socket and process packets here
    if((sk = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Socket Call");
        exit(-1);
    }
    //set up the socket
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(socknum);

    //bind the name (address) to a port
    if(bind(sk,(struct sockaddr*)&local, sizeof(local)) < 0){
        perror("bind call");
        exit(-1);
    }

    int messageLength = 0;
    char *packet = (char*)malloc(MAX_PACKET_SIZE);
    struct sockaddr_in remote;
    uint32_t rlen = sizeof(remote);

    if(getsockname(sk,(struct sockaddr*)&local,&len) < 0){
        perror("setsockname call");
        exit(-1);
    }
    printf("socket has port %d \n", ntohs(local.sin_port));

    // Wait for connection
    while (1) {
        // Wait for packets, and parse them as they come in
        // Can be 24 bytes or 1024, messageLength will be the determinant of what packet type it is
        messageLength = recvfrom(sk, packet, MAX_PACKET_SIZE, 0, (struct sockaddr*) &remote, &rlen);
        cout << "mesglen: " << messageLength << endl << "payload: " << packet << endl;
    }

}


// SendPackets will send the SAME packet to each of the incides, if you want to send a
// different packet to each of the nodes, that will need to be handled separately, or
// we cna leverage SendOnePacket
void Network::SendPacket(uint8_t *payload, const vector<int> indices, int payloadSize)
{
    for (int i = 0; i < indices.size(); i++)
    {
        NetInfo_t net_info;
        net_info.hostname = global_host_info[indices[i]].hostname;
        net_info.port = global_host_info[indices[i]].port;
        printf("sending packet to %s:%d\n", net_info.hostname.c_str(), net_info.port);
        net_info.payloadSize = payloadSize;
        // Malloc net_info.packet before? Not sure.  This seemed to have worked.
        memcpy(net_info.packet, payload, payloadSize);

        //std::thread t1(Network::SendOnePacket, std::ref(net_info));
        Network::SendOnePacket(net_info);
    }

}

void Network::SendOnePacket(NetInfo_t net_info)
{
    int sk = 0;
    struct sockaddr_in remote;
    struct hostent *hp;

    if((sk = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket call");
        exit(-1);
    }

    remote.sin_family = AF_INET;
    hp = gethostbyname(net_info.hostname.c_str());
    memcpy(&remote.sin_addr, hp->h_addr, hp->h_length);
    remote.sin_port = htons(net_info.port);

    sendto(sk, net_info.packet, net_info.packetSize, 0, (struct sockaddr*) &remote, sizeof(remote));
}



