#pragma once

#include "Debug/debug.hpp"

#include <enet/enet.h>
#include <format>

class PeerData {
    public:
    PeerData(uint8_t ID, ENetPeer* ENetPeer) : id(ID), enetpeer(ENetPeer) {}

    ENetPeer* getENetPeer() const {
        if (!enetpeer) {
            DEBUG_LOG("ENetPeer ID: " << static_cast<int>(id) << " is nullptr");
        }
        return enetpeer;
    }
    const bool isConnected() const {
        if (!enetpeer) {
            return false;
        }
        bool connected = enetpeer->state == ENET_PEER_STATE_CONNECTED ||
        enetpeer->state == ENET_PEER_STATE_CONNECTION_SUCCEEDED;
        return connected;
    }
    uint8_t getId() const { return id; }

    private:
    const int8_t id;
    ENetPeer* enetpeer;
};  