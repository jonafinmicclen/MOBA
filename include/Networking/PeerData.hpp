#pragma once

#include "Debug/debug.hpp"

#include <enet/enet.h>
#include <format>

#include <exception>

class PeerData {
    public:
    PeerData(ENetPeer* peer) : enetpeer(peer) {}

    ENetPeer* getENetPeer() const {
        if (!enetpeer) {
            DEBUG_LOG("ENetPeer ID: " << id << " is nullptr");
        }
        return enetpeer;
    }
    const bool isConnected() const {
        if (!enetpeer) {
            DEBUG_LOG("Connection query on null peer");
            return false;
        }
        bool connected = enetpeer->state == ENET_PEER_STATE_CONNECTED ||
        enetpeer->state == ENET_PEER_STATE_CONNECTION_SUCCEEDED;
        return connected;
    }
    std::string getId() const { 
        if (!idset) {
            DEBUG_LOG("ID accessed when it is not set.");
            throw std::runtime_error("Peer ID not set");
        }
        return id; 
    }
    void setID(std::string ID) {
        if (!idset) {
            id = ID;
        }
    } 

    private:
    bool idset;
    std::string id;
    ENetPeer* enetpeer = nullptr;
};  