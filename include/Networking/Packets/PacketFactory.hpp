#pragma once

#include "Networking/Packets/Packets.hpp"

#include <iostream>
#include <vector>
#include <memory>


namespace PacketFactory {

    template<typename pT>
    static std::unique_ptr<PacketBase> deserialise_packet(const uint8_t* bytes, size_t size) {
        auto packet = std::make_unique<pT>();
        packet->deserialize(bytes , size );
        return packet;
    }

    inline std::unique_ptr<PacketBase> packetFromBytes(const uint8_t* bytes, const size_t size) {
        if (size <= 1) {
            std::cerr<<"Attempted to construct packet but it was <= 1.\n";
            return nullptr;
        }
        const PacketType type = static_cast<PacketType>(bytes[0]);
        switch (type) {
            case PacketType::CastAbility:
                return deserialise_packet<CastAbilityPacket>(bytes, size);
            case PacketType::MoveCharacter:
                return deserialise_packet<MoveCharacterPacket>(bytes, size);
        }
        std::cerr<<"Attempted to construct unrecognised packet type. Type= " << static_cast<int>(bytes[0]);
        return nullptr;
    }

};