#pragma once

#include "Networking/Packets/AutoRegisterPacket.hpp"
#include "Common/Coordinates/WorldSpacePos.hpp"
#include "Game/Control/PlayerCommand.hpp"
#include "Debug/debug.hpp"

#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>


class ClientInputPacket
    : public AutoRegisterPacket<ClientInputPacket, PacketType::ClientInputPacket> {
public:
    static_assert(std::is_same_v<decltype(WorldSpacePos{}.x), float>,
                  "WorldSpacePos::x must be float for ClientInputPacket serialization");

    static_assert(std::is_same_v<decltype(WorldSpacePos{}.y), float>,
                  "WorldSpacePos::y must be float for ClientInputPacket serialization");

    static_assert(sizeof(float) == 4,
                  "ClientInputPacket expects 32-bit floats");

    static_assert(std::numeric_limits<float>::is_iec559,
                  "ClientInputPacket expects IEEE 754 floats");

    PacketType getType() const override {
        return PacketType::ClientInputPacket;
    }

    void deserialize(const uint8_t* data, size_t size) {
        if (size < PacketSize) {
            DEBUG_LOG("ClientInputPacket deserialize failed: packet too small");
            return;
        }

        data_.btn = static_cast<ClientInputButton>(data[0]);

        uint32_t x_bits =
            (static_cast<uint32_t>(data[1]) << 24) |
            (static_cast<uint32_t>(data[2]) << 16) |
            (static_cast<uint32_t>(data[3]) << 8) |
            static_cast<uint32_t>(data[4]);

        uint32_t y_bits =
            (static_cast<uint32_t>(data[5]) << 24) |
            (static_cast<uint32_t>(data[6]) << 16) |
            (static_cast<uint32_t>(data[7]) << 8) |
            static_cast<uint32_t>(data[8]);

        data_.mouse_pos.x = std::bit_cast<float>(x_bits);
        data_.mouse_pos.y = std::bit_cast<float>(y_bits);

        data_.release = data[9] != 0;
    }

    std::vector<uint8_t> serialize_() const {
        std::vector<uint8_t> d {};
        d.reserve(PacketSize);

        uint32_t x_bits = std::bit_cast<uint32_t>(data_.mouse_pos.x);
        uint32_t y_bits = std::bit_cast<uint32_t>(data_.mouse_pos.y);

        d.push_back(static_cast<uint8_t>(data_.btn));

        d.push_back(static_cast<uint8_t>((x_bits >> 24) & 0xFF));
        d.push_back(static_cast<uint8_t>((x_bits >> 16) & 0xFF));
        d.push_back(static_cast<uint8_t>((x_bits >> 8) & 0xFF));
        d.push_back(static_cast<uint8_t>(x_bits & 0xFF));

        d.push_back(static_cast<uint8_t>((y_bits >> 24) & 0xFF));
        d.push_back(static_cast<uint8_t>((y_bits >> 16) & 0xFF));
        d.push_back(static_cast<uint8_t>((y_bits >> 8) & 0xFF));
        d.push_back(static_cast<uint8_t>(y_bits & 0xFF));

        d.push_back(data_.release ? 1 : 0);

        return d;
    }

    ClientInput& getData() {
        return data_;
    }

    const ClientInput& getData() const {
        return data_;
    }

private:
    static constexpr size_t PacketSize = 10;

    ClientInput data_;
};