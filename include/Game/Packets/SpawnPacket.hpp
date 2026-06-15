#pragma once

#include "Networking/Packets/AutoRegisterPacket.hpp"
#include "Debug/debug.hpp"
#include "EntityComponentSystem/Components/Transform.hpp"
#include "EntityComponentSystem/Core/World.hpp"

#include <bit>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

struct SpawnCommand {
    std::string entity;
    Transform position;
    EntityHandle server_handle;
};

class SpawnPacket
    : public AutoRegisterPacket<SpawnPacket, PacketType::SpawnPacket> {
public:
    PacketType getType() const override {
        return PacketType::SpawnPacket;
    }

    const SpawnCommand& getData() const {
        return data_;
    }

    void setData(const SpawnCommand& data) {
        data_ = data;
    }

    void deserialize(const uint8_t* data, size_t size) {
        size_t offset = 0;

        auto require = [&](size_t amount) {
            if (offset + amount > size) {
                throw std::runtime_error("SpawnPacket deserialize failed: packet too small");
            }
        };

        auto read_u16 = [&]() -> uint16_t {
            require(2);

            uint16_t value =
                static_cast<uint16_t>(data[offset]) |
                static_cast<uint16_t>(data[offset + 1] << 8);

            offset += 2;
            return value;
        };

        auto read_f32 = [&]() -> float {
            require(4);

            uint32_t bits =
                static_cast<uint32_t>(data[offset]) |
                (static_cast<uint32_t>(data[offset + 1]) << 8) |
                (static_cast<uint32_t>(data[offset + 2]) << 16) |
                (static_cast<uint32_t>(data[offset + 3]) << 24);

            offset += 4;

            return std::bit_cast<float>(bits);
        };

        uint16_t entity_len = read_u16();

        require(entity_len);
        data_.entity.assign(
            reinterpret_cast<const char*>(data + offset),
            entity_len
        );
        offset += entity_len;

        data_.server_handle.eid = static_cast<EntityID>(read_u16());
        data_.server_handle.gen = static_cast<Generation>(read_u16());

        data_.position.position.x = read_f32();
        data_.position.position.y = read_f32();
        data_.position.position.z = read_f32();

        data_.position.rotation.w = read_f32();
        data_.position.rotation.x = read_f32();
        data_.position.rotation.y = read_f32();
        data_.position.rotation.z = read_f32();

        data_.position.scale.x = read_f32();
        data_.position.scale.y = read_f32();
        data_.position.scale.z = read_f32();
    }

    std::vector<uint8_t> serialize_() const {
        if (data_.entity.size() > std::numeric_limits<uint16_t>::max()) {
            throw std::runtime_error("SpawnPacket serialize failed: entity string too long");
        }

        std::vector<uint8_t> out;

        const uint16_t entity_len =
            static_cast<uint16_t>(data_.entity.size());

        // Reserve:
        // string length: 2
        // string bytes: entity_len
        // eid + gen: 4
        // transform: 10 floats * 4 bytes = 40
        out.reserve(2 + entity_len + 4 + 40);

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

        write_u16(entity_len);

        out.insert(
            out.end(),
            data_.entity.begin(),
            data_.entity.end()
        );

        write_u16(static_cast<uint16_t>(data_.server_handle.eid));
        write_u16(static_cast<uint16_t>(data_.server_handle.gen));

        write_f32(data_.position.position.x);
        write_f32(data_.position.position.y);
        write_f32(data_.position.position.z);

        write_f32(data_.position.rotation.w);
        write_f32(data_.position.rotation.x);
        write_f32(data_.position.rotation.y);
        write_f32(data_.position.rotation.z);

        write_f32(data_.position.scale.x);
        write_f32(data_.position.scale.y);
        write_f32(data_.position.scale.z);

        return out;
    }

private:
    SpawnCommand data_;
};