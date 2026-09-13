#pragma once

#include "Core/Networking/Packets/AutoRegisterPacket.hpp"
#include "Common/Debug/debug.hpp"
#include "Core/EntityComponentSystem/World.hpp"

#include <cstdint>
#include <vector>

// Tells clients to remove an entity - e.g. a homing projectile that hit its
// target or lost it (see ProjectileHomingSystem).
struct DespawnCommand {
    EntityHandle server_handle;
};

class DespawnPacket
    : public AutoRegisterPacket<DespawnPacket, PacketType::DespawnPacket> {
public:
    PacketType getType() const override {
        return PacketType::DespawnPacket;
    }

    const DespawnCommand& getData() const {
        return data_;
    }

    void setData(const DespawnCommand& data) {
        data_ = data;
    }

    void deserialize(const uint8_t* data, size_t size) {
        if (size < PacketSize) {
            DEBUG_LOG("DespawnPacket deserialize failed: packet too small");
            return;
        }

        auto read_u16 = [&](size_t offset) -> uint16_t {
            return static_cast<uint16_t>(data[offset]) |
                (static_cast<uint16_t>(data[offset + 1]) << 8);
        };

        data_.server_handle.eid = static_cast<EntityID>(read_u16(0));
        data_.server_handle.gen = static_cast<Generation>(read_u16(2));
    }

    std::vector<uint8_t> serialize_() const {
        std::vector<uint8_t> out;
        out.reserve(PacketSize);

        auto write_u16 = [&](uint16_t value) {
            out.push_back(static_cast<uint8_t>(value & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        };

        write_u16(static_cast<uint16_t>(data_.server_handle.eid));
        write_u16(static_cast<uint16_t>(data_.server_handle.gen));

        return out;
    }

private:
    static constexpr size_t PacketSize = 4;

    DespawnCommand data_;
};
