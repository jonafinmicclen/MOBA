// TestPacket.hpp
#pragma once

#include "Networking/Packets/PacketBase.hpp"
#include "Networking/Packets/PacketTypes.hpp"
#include "string"


class TestPacket : public PacketBase {
public:

    std::string packet_string;

    TestPacket(const std::string& str = "") : packet_string(str) {}

    PacketType getType() const override { return PacketType::TestPacket; }

    std::vector<uint8_t> serialize() const override {

        std::vector<uint8_t> data;
        data.push_back(static_cast<uint8_t>(getType()));
        uint32_t len = static_cast<uint32_t>(packet_string.size());
        data.push_back((len >> 24) & 0xFF);
        data.push_back((len >> 16) & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.push_back(len & 0xFF);
        for (auto const c : packet_string)  data.push_back(static_cast<uint8_t>(c));
        return data;
        
    }   

    void deserialize(const uint8_t* data, size_t size) override {

        if (size < 4) return;
        uint32_t len = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        if (size - 4 < len) return;
        packet_string = std::string(reinterpret_cast<const char*>(data + 4), len);

    }
    
};