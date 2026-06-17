#pragma once

#include "Common/Debug/debug.hpp"

#include "Networking/Packets/PacketBase.hpp"
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <cstddef>
#include <memory>

class PacketFactory {
public:
    using CreatorFunc = std::function<std::unique_ptr<PacketBase>()>;

    // Create or deserialize packet
    static std::unique_ptr<PacketBase> deserialisePacket(const uint8_t* bytes, size_t size) {
        if (size == 0) return nullptr;

        PacketType type = static_cast<PacketType>(bytes[0]);
        auto it = registry().find(type);
        if (it != registry().end()) {
            auto packet = it->second();
            packet->deserialize(bytes + 1, size - 1);
            return packet;
        }
        DEBUG_LOG("failed to create packet, type: " << (int)type);
        return nullptr;
    }

    static std::unique_ptr<PacketBase> deserialisePacket(const std::vector<uint8_t> bytes) {
        return deserialisePacket(bytes.data(), bytes.size());
    }

    // Register a packet type
    static void registerPacket(PacketType type, CreatorFunc creator) {
        registry()[type] = creator;
    }

private:
    // Single static registry map
    static std::unordered_map<PacketType, CreatorFunc>& registry() {
        static std::unordered_map<PacketType, CreatorFunc> instance;
        return instance;
    }
};
