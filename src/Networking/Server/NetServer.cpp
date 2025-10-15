#include "Networking/Server/NetServer.hpp"


NetServer::NetServer(const int max_client, const enet_uint32 host, const enet_uint16 port) {
    
    max_clients = max_client;

    address.host = host;
    address.port = port;

    if (enet_initialize () != 0)
    {
        throw std::runtime_error("Failed to initialize ENet");
    }

    server = enet_host_create(&address, max_clients, 2, 0, 0);

    if (!server) {
        enet_deinitialize();
        throw std::runtime_error("Failed to create ENet server");
    }
 
}

NetServer::~NetServer(){
    if (server) {
        enet_host_destroy(server);
        server = nullptr;
    }

    enet_deinitialize();
}