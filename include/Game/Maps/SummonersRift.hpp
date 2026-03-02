#pragma once

#include "Game/Maps/Map.hpp"
#include "Game/Maps/MapRegisterer.hpp"
#include "Game/Maps/MapFactory.hpp"


class SummonersRift : public Map {
public:
    SummonersRift() {
        Transform t;
        t.position = {0.0f,0.0f,17.0f};
        state = ObjectState{getName(), t};
    }
    std::string getName() override {return "Summoners Rift";}
};

// Header-only registration
inline MapRegisterer<SummonersRift> regSummonersRift("Summoners Rift");