#pragma once

#include <iostream>
#include <cstring>
#include <memory>

#include "Assets/ModelData.hpp"

class GLBLoader {
    public:
    GLBLoader(const char* path);
    ~GLBLoader();
    std::unique_ptr<Asset> asset_data = nullptr;

};