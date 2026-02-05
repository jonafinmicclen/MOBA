#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>   // for std::runtime_error
#include "Map.hpp"

class MapFactory {
public:
    static MapFactory& instance() {
        static MapFactory instance;
        return instance;
    }

    // Register a map with a name
    void registerMap(const std::string& name, std::function<std::unique_ptr<Map>()> creator) {
        registry[name] = creator;
    }

    // Create a map by name
    std::unique_ptr<Map> create(const std::string& name) {
        auto it = registry.find(name);
        if (it != registry.end()) {
            return it->second();
        }
        throw std::runtime_error("MapFactory: Unknown map '" + name + "'");
    }

private:
    MapFactory() = default;
    std::unordered_map<std::string, std::function<std::unique_ptr<Map>()>> registry;
};
