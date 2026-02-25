#pragma once

#include "Networking/Packets/AutoRegisterPacket.hpp"

#include <fstream>
#include "Debug/debug.hpp"

#include <optional>
#include <nlohmann/json.hpp>


using json = nlohmann::json;

class JSONPacket : public AutoRegisterPacket<JSONPacket, PacketType::JSON_Packet> {
    public:
    PacketType getType() const override {
        return PacketType::JSON_Packet;
    }

    void deserialize(const uint8_t* data, size_t size) {
        std::string s(reinterpret_cast<const char*>(data), size);
        json_data = json::parse(s);
    }

    std::vector<uint8_t> serialize_() const {
        std::string d = json_data.dump();
        std::vector<uint8_t> bytes(d.begin(), d.end());
        return bytes;
    }

    void json_from_string(const std::string str) {
        json_data = json::parse(str);
    }

    void json_from_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            DEBUG_LOG("Failed to open JSON file: " << path);
            return;
        }

        try {
            file >> json_data;
        } catch (const json::parse_error& e) {
            DEBUG_LOG("JSON parse error in file " << path << ": " << e.what());
        }
    }

    const json& get_json() const {
        return json_data;
    }

    void set_json(json& data) {
        json_data = data;
    }



    protected:
    json json_data;
};