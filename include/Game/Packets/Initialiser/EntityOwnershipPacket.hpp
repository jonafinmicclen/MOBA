#pragma once

#include "Networking/Packets/AutoRegisterPacket.hpp"
#include "Common/Debug/debug.hpp"
#include "EntityComponentSystem/World.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

struct EntityOwnershipUpdate {
    EntityHandle server_handle;
};

class EntityOwnershipPacket
    : public AutoRegisterPacket<EntityOwnershipPacket, PacketType::EntityOwnershipPacket> {
public:
    PacketType getType() const override {
        return PacketType::EntityOwnershipPacket;
    }

    const EntityOwnershipUpdate& getData() const {
        return data_;
    }

    void setData(const EntityOwnershipUpdate& data) {
        data_ = data;
    }

    void deserialize(const uint8_t* data, size_t size) {
        size_t offset = 0;

        auto require = [&](size_t amount) {
            if (offset + amount > size) {
                throw std::runtime_error(
                    "EntityOwnershipPacket deserialize failed: packet too small"
                );
            }
        };

        auto read_u16 = [&]() -> uint16_t {
            require(2);

            uint16_t value =
                static_cast<uint16_t>(data[offset]) |
                static_cast<uint16_t>(
                    static_cast<uint16_t>(data[offset + 1]) << 8
                );

            offset += 2;
            return value;
        };

        data_ = EntityOwnershipUpdate{};

        data_.server_handle.eid = static_cast<EntityID>(read_u16());
        data_.server_handle.gen = static_cast<Generation>(read_u16());
    }

    std::vector<uint8_t> serialize_() const {
        std::vector<uint8_t> out;
        out.reserve(4);

        auto write_u16 = [&](uint16_t value) {
            out.push_back(static_cast<uint8_t>(value & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        };

        write_u16(static_cast<uint16_t>(data_.server_handle.eid));
        write_u16(static_cast<uint16_t>(data_.server_handle.gen));

        return out;
    }

private:
    EntityOwnershipUpdate data_;
};