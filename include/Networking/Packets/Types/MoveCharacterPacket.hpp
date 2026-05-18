// MoveCharacterPacket.hpp
#pragma once

#include "Networking/Packets/PacketBase.hpp"
#include "Networking/Packets/Types/PacketTypes.hpp"

#include "Game/Events/MoveCharacterEvent.hpp"

#include <cstring>
#include <iostream>

class MoveCharacterPacket : public PacketBase {
public:

    PacketType getType() const override { return PacketType::MoveCharacter; }
    std::unique_ptr<MoveCharacterEvent> event;

    
    std::vector<uint8_t> serialize() const override {
        if (!event) {
            throw std::runtime_error("MoveCharacterPacket: event is nullptr, cannot serialize");
        }

        std::cout<<"attem,ping"<<std::endl;

        std::vector<uint8_t> data;

        // Serialize CharacterID.id
        int id = event->id.id;
        uint8_t* idPtr = reinterpret_cast<uint8_t*>(&id);
        data.insert(data.end(), idPtr, idPtr + sizeof(int));

        // Serialize WorldSpacePos.x
        int x = event->pos.x;
        uint8_t* xPtr = reinterpret_cast<uint8_t*>(&x);
        data.insert(data.end(), xPtr, xPtr + sizeof(int));

        // Serialize WorldSpacePos.y
        int y = event->pos.y;
        uint8_t* yPtr = reinterpret_cast<uint8_t*>(&y);
        data.insert(data.end(), yPtr, yPtr + sizeof(int));

        return data;
    }

    void deserialize(const uint8_t* data, size_t size) override {
        if (size < sizeof(int) * 3) {
            throw std::runtime_error("MoveCharacterPacket: Not enough data to deserialize");
        }

        if (!event) {
            event = std::make_unique<MoveCharacterEvent>();
        }

        size_t offset = 0;

        // Deserialize CharacterID.id
        std::memcpy(&event->id.id, data + offset, sizeof(int));
        offset += sizeof(int);

        // Deserialize WorldSpacePos.x
        std::memcpy(&event->pos.x, data + offset, sizeof(int));
        offset += sizeof(int);

        // Deserialize WorldSpacePos.y
        std::memcpy(&event->pos.y, data + offset, sizeof(int));
        offset += sizeof(int);
    }
    
    
};