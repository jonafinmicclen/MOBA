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

NetServer::~NetServer() {
    if (server) {
        enet_host_destroy(server);
        server = nullptr;
    }

    enet_deinitialize();
}

void NetServer::pollEvents() {
    /* Wait up to 1000 milliseconds for an event. */
    while (enet_host_service (server, &event, 1000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf ("A new client connected from %x:%u.\n", 
                    event.peer -> address.host,
                    event.peer -> address.port);
    
            /* Store any relevant client information here. */
            //event.peer -> data = "Client information";
    
            break;
    
        case ENET_EVENT_TYPE_RECEIVE:
            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    event.peer -> data,
                    event.channelID);
    
            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event.packet);
            
            break;
        
        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconnected.\n", event.peer -> data);
    
            /* Reset the peer's client information. */
    
            event.peer -> data = NULL;
        }
    }
}

void NetServer::sendPackets() {
    // Prepare the message using std::string
    std::string message = "Packet of cock and balls";

    // Create a reliable ENet packet
    ENetPacket* packet = enet_packet_create(
        message.c_str(),              // pointer to the data
        message.size() + 1,           // include null terminator
        ENET_PACKET_FLAG_RELIABLE     // reliable delivery
    );

    enet_host_broadcast (server, 0, packet);

    // Flush immediately if needed
    enet_host_flush(server);
}