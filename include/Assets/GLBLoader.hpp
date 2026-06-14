#pragma once

#include <iostream>
#include <cstring>
#include <memory>
#include <optional>
#include "external/cgltf.h"    
#include "Assets/ModelData.hpp"

class GLBLoader {
    public:
    GLBLoader(const char* path);
    ~GLBLoader();
    std::optional<Asset> asset_data;
    std::optional<Texture> LoadTexture(cgltf_image* img);
};