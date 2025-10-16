// NetClient.hpp
#pragma once

#include <enet/enet.h>
#include <iostream>


class NetClient {
public:

NetClient(const char* server_add, int port);
~NetClient();

void pollEvents();
void connectServer();
void sendPackets();


private:

const char* server_address;
int server_port;

ENetHost* client;
ENetAddress address;
ENetPeer *peer;
ENetEvent event;
};