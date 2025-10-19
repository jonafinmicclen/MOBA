#pragma once

#include "Networking/Packets/PacketBase.hpp"

// All subpackets
#include "Networking/Packets/TestPacket.hpp"
//#include "Networking/Packets/PlayerInputPacket.hpp"

#include <vector>


namespace PacketFactory {

    template<typename pT>
    inline void deserialise_packet(PacketBase** packet, const uint8_t* bytes, const size_t size) {
        *packet = new pT;
        (*packet)->deserialize(bytes + 1, size - 1);
    }

    inline PacketBase* packetFromBytes(const uint8_t* bytes, const size_t size) {
        const PacketType type = static_cast<PacketType>(bytes[0]);
        PacketBase* packet = nullptr;
        switch (type) {
            //case PacketType::PlayerInput:
            //    packet = new PlayerInputPacket;
            //    packet->deserialize(bytes, size);
            //    break;
            case PacketType::TestPacket:
                deserialise_packet<TestPacket>(&packet, bytes, size);
                break;
        }
        return packet;
    }

};