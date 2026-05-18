#include "Assets/GLBLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define CGLTF_IMPLEMENTATION
#include "external/cgltf.h"

#include <iostream>

GLBLoader::GLBLoader(const char* path) {
    cgltf_data* data = nullptr;
    cgltf_options options{};

    auto asset = std::make_unique<Asset>();

    cgltf_result result = cgltf_parse_file(&options, path, &data);
    if (result != cgltf_result_success) {
        std::cerr << "[GLBLoader] Failed to parse GLB: " << result << std::endl;
        return;
    }

    result = cgltf_load_buffers(&options, data, path);
    if (result != cgltf_result_success) {
        std::cerr << "[GLBLoader] Failed to load buffers: " << result << std::endl;
        cgltf_free(data);
        return;
    }

    // ----------------------
    // Extract meshes
    // ----------------------
    if (data->meshes_count > 0) {
        asset->mesh = std::make_unique<MeshData>();

        for (size_t m = 0; m < data->meshes_count; ++m) {
            cgltf_mesh& cgMesh = data->meshes[m];

            for (size_t p = 0; p < cgMesh.primitives_count; ++p) {
                cgltf_primitive& prim = cgMesh.primitives[p];

                // Material / texture index for this primitive
                int texIndex = -1;
                if (prim.material) {
                    cgltf_material* mat = prim.material;
                    auto material = std::make_shared<Material>();

                    if (mat->has_pbr_metallic_roughness && mat->pbr_metallic_roughness.base_color_texture.texture) {
                        material->albedo = LoadTexture(mat->pbr_metallic_roughness.base_color_texture.texture->image);
                        material->albedo->uvIndex = mat->pbr_metallic_roughness.base_color_texture.texcoord;
                    }
                    if (material->albedo) {
                        asset->materials.push_back(material);
                        asset->mesh->textures.push_back(material->albedo);
                        texIndex = static_cast<int>(asset->mesh->textures.size() - 1);
                    }
                }

                // Determine vertex count
                size_t vertexCount = 0;
                for (size_t a = 0; a < prim.attributes_count; ++a) {
                    if (prim.attributes[a].type == cgltf_attribute_type_position) {
                        vertexCount = prim.attributes[a].data->count;
                        break;
                    }
                }

                size_t baseIndex = asset->mesh->vertices.size();
                asset->mesh->vertices.resize(baseIndex + vertexCount);

                // Extract attributes
                for (size_t a = 0; a < prim.attributes_count; ++a) {
                    cgltf_attribute& attr = prim.attributes[a];
                    cgltf_accessor* accessor = attr.data;

                    if (attr.type == cgltf_attribute_type_position) {
                        for (size_t v = 0; v < accessor->count; ++v) {
                            float pos[3];
                            cgltf_accessor_read_float(accessor, v, pos, 3);
                            asset->mesh->vertices[baseIndex + v].position = glm::vec3(pos[0], pos[1], pos[2]);
                        }
                    } 
                    else if (attr.type == cgltf_attribute_type_normal) {
                        for (size_t v = 0; v < accessor->count; ++v) {
                            float norm[3];
                            cgltf_accessor_read_float(accessor, v, norm, 3);
                            asset->mesh->vertices[baseIndex + v].normal = glm::vec3(norm[0], norm[1], norm[2]);
                        }
                    } 
                    else if (attr.type == cgltf_attribute_type_texcoord) {
                        int uvSet = attr.index; // only support first set
                        for (size_t v = 0; v < accessor->count; ++v) {
                            float uv[2];
                            cgltf_accessor_read_float(accessor, v, uv, 2);
                            if (uvSet == 0)
                                asset->mesh->vertices[baseIndex + v].uv = glm::vec2(uv[0], uv[1]);
                        }
                    }
                }

                // Extract indices
                if (prim.indices) {
                    cgltf_accessor* indexAccessor = prim.indices;
                    size_t indexOffset = asset->mesh->indices.size();
                    asset->mesh->indices.resize(indexOffset + indexAccessor->count);
                    asset->mesh->faceTextureIndices.resize(indexOffset / 3 + indexAccessor->count / 3);

                    for (size_t i = 0; i < indexAccessor->count; ++i) {
                        unsigned int idx = 0;
                        cgltf_accessor_read_uint(indexAccessor, i, &idx, sizeof(cgltf_uint));
                        asset->mesh->indices[indexOffset + i] = idx + baseIndex;

                        // Assign texture per triangle
                        if (i % 3 == 2) { // last vertex of triangle
                            size_t faceID = (indexOffset + i) / 3;
                            asset->mesh->faceTextureIndices[faceID] = texIndex;
                        }
                    }
                }
            }
        }
    }

    // ----------------------
    // Extract animations (unchanged)
    // ----------------------
    if (data->animations_count > 0) {
        for (size_t a = 0; a < data->animations_count; ++a) {
            cgltf_animation* cgAnim = &data->animations[a];
            AnimationData anim;
            anim.name = cgAnim->name ? cgAnim->name : "Unnamed";

            for (size_t c = 0; c < cgAnim->channels_count; ++c) {
                cgltf_animation_channel* channel = &cgAnim->channels[c];
                cgltf_animation_sampler* sampler = channel->sampler;
                NodeAnimation nodeAnim;
                nodeAnim.nodeName = channel->target_node && channel->target_node->name
                                    ? channel->target_node->name
                                    : "UnnamedNode";

                for (size_t k = 0; k < sampler->input->count; ++k) {
                    Keyframe key;
                    cgltf_accessor_read_float(sampler->input, k, &key.time, 1);

                    if (channel->target_path == cgltf_animation_path_type_translation) {
                        float val[3];
                        cgltf_accessor_read_float(sampler->output, k, val, 3);
                        key.translation = glm::vec3(val[0], val[1], val[2]);
                    } 
                    else if (channel->target_path == cgltf_animation_path_type_rotation) {
                        float val[4];
                        cgltf_accessor_read_float(sampler->output, k, val, 4);
                        key.rotation = glm::quat(val[3], val[0], val[1], val[2]);
                    } 
                    else if (channel->target_path == cgltf_animation_path_type_scale) {
                        float val[3];
                        cgltf_accessor_read_float(sampler->output, k, val, 3);
                        key.scale = glm::vec3(val[0], val[1], val[2]);
                    }

                    nodeAnim.keyframes.push_back(key);
                }

                anim.channels.push_back(nodeAnim);
            }

            asset->animations.push_back(anim);
        }
    }

    asset_data = std::move(asset);

    if (data)
        cgltf_free(data);
}

GLBLoader::~GLBLoader() {}

std::shared_ptr<Texture> GLBLoader::LoadTexture(cgltf_image* img) {
    if (!img) return nullptr;

    std::vector<unsigned char> imageBytes;

    if (img->buffer_view) {
        cgltf_buffer_view* bv = img->buffer_view;
        uint8_t* src = (uint8_t*)bv->buffer->data + bv->offset;
        imageBytes.assign(src, src + bv->size);
    } else if (img->uri) {
        std::cerr << "[GLBLoader] External texture URIs not supported: " << img->uri << std::endl;
        return nullptr;
    }

    int width, height, channels;
    unsigned char* decoded = stbi_load_from_memory(imageBytes.data(), static_cast<int>(imageBytes.size()),
                                                   &width, &height, &channels, 4);
    if (!decoded) {
        std::cerr << "[GLBLoader] Failed to decode texture." << std::endl;
        return nullptr;
    }

    auto texture = std::make_shared<Texture>();
    texture->width = width;
    texture->height = height;
    texture->channels = 4;
    texture->uvIndex = 0; // first UV set
    texture->data.assign(decoded, decoded + (width * height * 4));
    stbi_image_free(decoded);

    return texture;
}
