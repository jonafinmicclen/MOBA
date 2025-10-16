#include "Networking/Client/NetClient.hpp"


NetClient::NetClient(const char* server_add, int port) {

    server_address = server_add;
    server_port = port;

    client = enet_host_create (NULL /* create a client host */,
            1 /* only allow 1 outgoing connection */,
            2 /* allow up 2 channels to be used, 0 and 1 */,
            0 /* assume any amount of incoming bandwidth */,
            0 /* assume any amount of outgoing bandwidth */);
 
    if (!client) {
        enet_deinitialize();
        throw std::runtime_error("Failed to create ENet client");
    }

    connectServer();
}

NetClient::~NetClient() {
    enet_host_destroy(client);
}

void NetClient::pollEvents() {
    if (enet_host_service (client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("Connection attempted to client which should have been already connected.");
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

void NetClient::connectServer() {
    /* Connect to some.server.net:1234. */
    enet_address_set_host (& address, server_address);
    address.port = server_port;
    
    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect (client, & address, 2, 0);    
    
    if (peer == NULL)
    {
    fprintf (stderr, 
                "No available peers for initiating an ENet connection.\n");
    exit (EXIT_FAILURE);
    }
    
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service (client, & event, 10000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts ("Connection to some.server.net:1234 succeeded.");
    }
    else
    {
        enet_peer_reset (peer);
        puts ("Connection to server failed.");

    }
}

void NetClient::sendPackets() {
    // Prepare the message using std::string
    std::string message = "client packet";

    // Create a reliable ENet packet
    ENetPacket* packet = enet_packet_create(
        message.c_str(),              // pointer to the data
        message.size() + 1,           // include null terminator
        ENET_PACKET_FLAG_RELIABLE     // reliable delivery
    );

    // Send the packet over channel 0
    enet_peer_send(peer, 0, packet);

    // Flush immediately if needed
    enet_host_flush(client);
}