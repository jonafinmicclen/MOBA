#pragma once
#include "MapFactory.hpp"

template<typename T>
struct MapRegisterer {
    MapRegisterer(const std::string& name) {
        MapFactory::instance().registerMap(name, []() { 
            return std::make_unique<T>(); 
        });
    }
};
