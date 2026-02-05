#pragma once

#include "Game/Maps/Map.hpp"
#include "Game/Maps/MapRegisterer.hpp"
#include "Game/Maps/MapFactory.hpp"


class SummonersRift : public Map {
    public:
    std::string getName() override {return "Summoners Rift";}
};