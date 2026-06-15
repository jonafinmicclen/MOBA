#pragma once

#include "Networking/Packets/AutoRegisterPacket.hpp"
#include "Debug/debug.hpp"
#include "EntityComponentSystem/Core/World.hpp"
#include "EntityComponentSystem/Components/Transform.hpp"

#include <bit>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

enum class EntityFieldIdentifier : uint32_t {
    Transform = 1 << 0,
    // Path   = 1 << 1,
    // Health = 1 << 2,
    // Team   = 1 << 3,
};

inline EntityFieldIdentifier operator|(
    EntityFieldIdentifier a,
    EntityFieldIdentifier b
) {
    return static_cast<EntityFieldIdentifier>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

inline EntityFieldIdentifier operator&(
    EntityFieldIdentifier a,
    EntityFieldIdentifier b
) {
    return static_cast<EntityFieldIdentifier>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

inline bool hasField(
    EntityFieldIdentifier mask,
    EntityFieldIdentifier field
) {
    return static_cast<uint32_t>(mask & field) != 0;
}

struct EntityStateUpdate {
    EntityHandle handle;
    EntityFieldIdentifier affected_fields = {};

    Transform new_transform;
};

class EntityStatePacket
    : public AutoRegisterPacket<EntityStatePacket, PacketType::EntityStatePacket> {
public:
    PacketType getType() const override {
        return PacketType::EntityStatePacket;
    }

    const EntityStateUpdate& getData() const {
        return data_;
    }

    void setData(const EntityStateUpdate& data) {
        data_ = data;
    }

    void deserialize(const uint8_t* data, size_t size) {
        size_t offset = 0;

        auto require = [&](size_t amount) {
            if (offset + amount > size) {
                throw std::runtime_error(
                    "EntityStatePacket deserialize failed: packet too small"
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

        auto read_u32 = [&]() -> uint32_t {
            require(4);

            uint32_t value =
                static_cast<uint32_t>(data[offset]) |
                (static_cast<uint32_t>(data[offset + 1]) << 8) |
                (static_cast<uint32_t>(data[offset + 2]) << 16) |
                (static_cast<uint32_t>(data[offset + 3]) << 24);

            offset += 4;
            return value;
        };

        auto read_f32 = [&]() -> float {
            uint32_t bits = read_u32();
            return std::bit_cast<float>(bits);
        };

        data_ = EntityStateUpdate{};

        // Always present.
        data_.handle.eid = static_cast<EntityID>(read_u16());
        data_.handle.gen = static_cast<Generation>(read_u16());

        data_.affected_fields =
            static_cast<EntityFieldIdentifier>(read_u32());

        // Optional fields, in fixed order.
        if (hasField(data_.affected_fields, EntityFieldIdentifier::Transform)) {
            data_.new_transform.position.x = read_f32();
            data_.new_transform.position.y = read_f32();
            data_.new_transform.position.z = read_f32();

            data_.new_transform.rotation.w = read_f32();
            data_.new_transform.rotation.x = read_f32();
            data_.new_transform.rotation.y = read_f32();
            data_.new_transform.rotation.z = read_f32();

            data_.new_transform.scale.x = read_f32();
            data_.new_transform.scale.y = read_f32();
            data_.new_transform.scale.z = read_f32();
        }
    }

    std::vector<uint8_t> serialize_() const {
        std::vector<uint8_t> out;

        // handle: 4 bytes
        // affected_fields: 4 bytes
        // transform: 10 floats * 4 bytes = 40 bytes if present
        size_t reserve_size = 4 + 4;

        if (hasField(data_.affected_fields, EntityFieldIdentifier::Transform)) {
            reserve_size += 40;
        }

        out.reserve(reserve_size);

        auto write_u16 = [&](uint16_t value) {
            out.push_back(static_cast<uint8_t>(value & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        };

        auto write_u32 = [&](uint32_t value) {
            out.push_back(static_cast<uint8_t>(value & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
            out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
        };

        auto write_f32 = [&](float value) {
            uint32_t bits = std::bit_cast<uint32_t>(value);
            write_u32(bits);
        };

        // Always present.
        write_u16(static_cast<uint16_t>(data_.handle.eid));
        write_u16(static_cast<uint16_t>(data_.handle.gen));

        write_u32(static_cast<uint32_t>(data_.affected_fields));

        // Optional fields, in fixed order.
        if (hasField(data_.affected_fields, EntityFieldIdentifier::Transform)) {
            write_f32(data_.new_transform.position.x);
            write_f32(data_.new_transform.position.y);
            write_f32(data_.new_transform.position.z);

            write_f32(data_.new_transform.rotation.w);
            write_f32(data_.new_transform.rotation.x);
            write_f32(data_.new_transform.rotation.y);
            write_f32(data_.new_transform.rotation.z);

            write_f32(data_.new_transform.scale.x);
            write_f32(data_.new_transform.scale.y);
            write_f32(data_.new_transform.scale.z);
        }

        return out;
    }

private:
    EntityStateUpdate data_;
};