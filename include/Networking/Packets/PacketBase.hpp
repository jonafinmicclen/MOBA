#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Networking/Packets/Types/PacketTypes.hpp"
#include "Game/Events/BaseEvent.hpp"


class PacketBase {
public:

    virtual ~PacketBase() {};
    virtual PacketType getType() const = 0;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(const uint8_t* data, size_t size) = 0;
};
