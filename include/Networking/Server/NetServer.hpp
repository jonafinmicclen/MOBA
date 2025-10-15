// NetServer.hpp
#pragma once

#include <enet/enet.h>
#include <iostream>


class NetServer {
    public:
    NetServer(const int max_client, const enet_uint32 host = ENET_HOST_ANY, const enet_uint16 port = 1234);
    ~NetServer();

    private:

    ENetAddress address;
    ENetHost* server;

    int max_clients;

};