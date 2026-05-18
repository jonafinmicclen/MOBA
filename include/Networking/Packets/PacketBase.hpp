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
    virtual void deserialize(const uint8_t* data, size_t size) = 0;
<<<<<<< HEAD

    virtual std::vector<uint8_t> serialize() const {
        auto b = serialize_();
        b.insert(b.begin(), static_cast<uint8_t>(getType()));
        return b;
    };
    
    protected:
    virtual std::vector<uint8_t> serialize_() const = 0;
=======
    std::unique_ptr<BaseEvent> event = nullptr;
>>>>>>> origin/main
};
