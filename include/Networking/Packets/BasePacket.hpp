// BasePacket.hpp
#pragma once

#include <cstdint>
#include <vector>

#include "Networking/Packets/PacketTypes.hpp"


class BasePacket {
public:
    virtual ~BasePacket() {};
    virtual PacketType getType() const = 0;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(const uint8_t* data, size_t size) = 0;
};
