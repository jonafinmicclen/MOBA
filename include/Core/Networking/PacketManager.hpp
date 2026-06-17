#pragma once

#include "Core/Networking/Core/Networker.hpp"
#include "Core/Networking/Packets/PacketDistributor.hpp"
#include "Core/Networking/Packets/PacketFactory.hpp"

#include "Common/Debug/debug.hpp"

class PacketManager {
public:
    PacketManager(PacketDistributor& distributor, Networker& networker) 
        : distributor_(distributor), networker_(networker) {}


    /**
     * Reads from networker and dispatches all packets via the distributor.
     */
    void pump() {
        while (auto msg = networker_.popMessage()) {
            std::unique_ptr<PacketBase> pkt = PacketFactory::deserialisePacket(msg->bytes);
            PacketMetadata metadata(*msg);

            if (!pkt) {
                continue;
            }

            //DEBUG_LOG("Packet Dispatched");

            distributor_.dispatch(pkt, metadata);
        }
    }

private:
    const PacketDistributor& distributor_;
    Networker& networker_;
};