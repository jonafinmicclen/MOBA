#pragma once

#include "Core/Networking/Packets/AutoRegisterPacket.hpp"
#include "Game/Components/Transform.hpp"
#include "Core/EntityComponentSystem/World.hpp"
#include "Common/Debug/debug.hpp"

#include <bit>
#include <cstdint>
#include <vector>

// Tells clients to spawn a projectile - deliberately separate from
// SpawnPacket/SpawnCommand (which is champion-shaped: carries an asset
// name string for mesh resolution, plus ownership implications that don't
// apply to a projectile nobody controls). Wire format is fixed-size since
// there's no variable-length name field to carry - the visual for now is a
// hardcoded placeholder (see DuplicationSystem::projectileSpawnHandler).
struct ProjectileSpawnCommand {
    Transform position;
    EntityHandle server_handle;
};

class ProjectileSpawnPacket
    : public AutoRegisterPacket<ProjectileSpawnPacket, PacketType::ProjectileSpawnPacket> {
public:
    PacketType getType() const override {
        return PacketType::ProjectileSpawnPacket;
    }

    const ProjectileSpawnCommand& getData() const {
        return data_;
    }

    void setData(const ProjectileSpawnCommand& data) {
        data_ = data;
    }

    void deserialize(const uint8_t* data, size_t size) {
        if (size < PacketSize) {
            DEBUG_LOG("ProjectileSpawnPacket deserialize failed: packet too small");
            return;
        }

        size_t offset = 0;

        auto read_u16 = [&]() -> uint16_t {
            uint16_t value =
                static_cast<uint16_t>(data[offset]) |
                (static_cast<uint16_t>(data[offset + 1]) << 8);
            offset += 2;
            return value;
        };

        auto read_f32 = [&]() -> float {
            uint32_t bits =
                static_cast<uint32_t>(data[offset]) |
                (static_cast<uint32_t>(data[offset + 1]) << 8) |
                (static_cast<uint32_t>(data[offset + 2]) << 16) |
                (static_cast<uint32_t>(data[offset + 3]) << 24);
            offset += 4;
            return std::bit_cast<float>(bits);
        };

        auto read_fixed = [&]() -> Fixed {
            uint32_t bits =
                static_cast<uint32_t>(data[offset]) |
                (static_cast<uint32_t>(data[offset + 1]) << 8) |
                (static_cast<uint32_t>(data[offset + 2]) << 16) |
                (static_cast<uint32_t>(data[offset + 3]) << 24);
            offset += 4;
            return Fixed::fromRaw(std::bit_cast<int32_t>(bits));
        };

        data_.position.position.x = read_fixed();
        data_.position.position.y = read_fixed();
        data_.position.position.z = read_fixed();

        data_.position.rotation.w = read_f32();
        data_.position.rotation.x = read_f32();
        data_.position.rotation.y = read_f32();
        data_.position.rotation.z = read_f32();

        data_.position.scale.x = read_f32();
        data_.position.scale.y = read_f32();
        data_.position.scale.z = read_f32();

        data_.server_handle.eid = static_cast<EntityID>(read_u16());
        data_.server_handle.gen = static_cast<Generation>(read_u16());
    }

    std::vector<uint8_t> serialize_() const {
        std::vector<uint8_t> out;
        out.reserve(PacketSize);

        auto write_u16 = [&](uint16_t value) {
            out.push_back(static_cast<uint8_t>(value & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        };

        auto write_f32 = [&](float value) {
            uint32_t bits = std::bit_cast<uint32_t>(value);
            out.push_back(static_cast<uint8_t>(bits & 0xFF));
            out.push_back(static_cast<uint8_t>((bits >> 8) & 0xFF));
            out.push_back(static_cast<uint8_t>((bits >> 16) & 0xFF));
            out.push_back(static_cast<uint8_t>((bits >> 24) & 0xFF));
        };

        auto write_fixed = [&](Fixed value) {
            uint32_t bits = std::bit_cast<uint32_t>(value.raw());
            out.push_back(static_cast<uint8_t>(bits & 0xFF));
            out.push_back(static_cast<uint8_t>((bits >> 8) & 0xFF));
            out.push_back(static_cast<uint8_t>((bits >> 16) & 0xFF));
            out.push_back(static_cast<uint8_t>((bits >> 24) & 0xFF));
        };

        write_fixed(data_.position.position.x);
        write_fixed(data_.position.position.y);
        write_fixed(data_.position.position.z);

        write_f32(data_.position.rotation.w);
        write_f32(data_.position.rotation.x);
        write_f32(data_.position.rotation.y);
        write_f32(data_.position.rotation.z);

        write_f32(data_.position.scale.x);
        write_f32(data_.position.scale.y);
        write_f32(data_.position.scale.z);

        write_u16(static_cast<uint16_t>(data_.server_handle.eid));
        write_u16(static_cast<uint16_t>(data_.server_handle.gen));

        return out;
    }

private:
    // 3 Fixed (12) + 4 float rotation (16) + 3 float scale (12) + eid/gen (4)
    static constexpr size_t PacketSize = 44;

    ProjectileSpawnCommand data_;
};
