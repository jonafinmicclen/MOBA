#pragma once

#include "Networking/Core/Networker.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "Networking/Packets/PacketFactory.hpp"

class PacketManager {
public:
    PacketManager(PacketFactory& factory, PacketDistributor& distributor, Networker& networker) 
        : factory_(factory), distributor_(distributor), networker_(networker) {}


    /**
     * Reads from networker and dispatches all packets via the distributor.
     */
    void pump() {
        while (auto msg = networker_.popMessage()) {
            std::unique_ptr<PacketBase> pkt = factory_.deserialisePacket(msg->bytes);
            PacketMetadata metadata(*msg);

            if (!pkt) {
                continue;
            }

            distributor_.dispatch(pkt, metadata);
        }
    }

private:
    const PacketFactory& factory_;
    const PacketDistributor& distributor_;
    Networker& networker_;
};