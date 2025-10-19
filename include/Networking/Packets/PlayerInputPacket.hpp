// PlayerInputPacket.hpp
#pragma once

#include "Networking/Packets/PacketBase.hpp"
#include "Networking/Packets/PacketTypes.hpp"

class PlayerInputPacket : public PacketBase {
public:

    uint8_t client_id;
    uint8_t champion_id;
    uint32_t x;
    uint32_t y;

    PacketType getType() const override { return PacketType::PlayerInput; }

    std::vector<uint8_t> serialize() const override {

    }   

    void deserialize(const uint8_t* data, size_t size) override {
        
    }
    
};