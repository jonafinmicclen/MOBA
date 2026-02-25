#pragma once

#include <cstdint>
#include <vector>

#include "Networking/Packets/PacketTypes.hpp"


class PacketBase {
public:
    virtual ~PacketBase() {};
    virtual PacketType getType() const = 0;
    virtual void deserialize(const uint8_t* data, size_t size) = 0;

    virtual std::vector<uint8_t> serialize() const {
        auto b = serialize_();
        b.insert(b.begin(), static_cast<uint8_t>(getType()));
        return b;
    };
    
    protected:
    virtual std::vector<uint8_t> serialize_() const = 0;
};
