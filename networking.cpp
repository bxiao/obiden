#pragma once

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

void Network::SetupNetwork()
{
    FILE *configFile;

    configFile = fopen("setup.config", "r");

    parseContents(configFile);
    fclose(configFile);

    socket_addrinfo = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    len = sizeof(*socket_addrinfo);

    kart_setup(player);
}
