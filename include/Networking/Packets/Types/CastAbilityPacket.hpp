// CastAbilityPacket.hpp
#pragma once

#include "Networking/Packets/PacketBase.hpp"
#include "Game/Events/CastAbilityEvent.hpp"

#include <memory>


class CastAbilityPacket : public PacketBase {
public:

    PacketType getType() const override { return PacketType::CastAbility; }
    std::unique_ptr<CastAbilityEvent> event;

    
    std::vector<uint8_t> serialize() const override {

    }   
    void deserialize(const uint8_t* data, size_t size) override {
        
    }
    
};