#include "Assets/GLBLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define CGLTF_IMPLEMENTATION
#include "external/cgltf.h"

GLBLoader::GLBLoader(const char* path) {
    cgltf_data* data;
    cgltf_options options = {};

    // Initialize asset
    std::unique_ptr<Asset> asset = std::make_unique<Asset>();

    // Parse GLB file
    cgltf_result result = cgltf_parse_file(&options, path, &data);
    if (result != cgltf_result_success) {
        std::cerr << "[GLBLoader] Failed to parse GLB: " << result << std::endl;
        return;
    }

    // Load buffers (binary data)
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

        for (size_t i = 0; i < data->meshes_count; ++i) {
            cgltf_mesh& cgMesh = data->meshes[i];

            for (size_t p = 0; p < cgMesh.primitives_count; ++p) {
                cgltf_primitive& prim = cgMesh.primitives[p];

                // Determine vertex count
                size_t vertexCount = 0;
                for (size_t a = 0; a < prim.attributes_count; ++a) {
                    if (prim.attributes[a].type == cgltf_attribute_type_position) {
                        vertexCount = prim.attributes[a].data->count;
                        break;
                    }
                }

                // Base index for combining primitives
                size_t baseIndex = asset->mesh->vertices.size();
                asset->mesh->vertices.resize(baseIndex + vertexCount);

                // Extract vertex attributes
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
                        for (size_t v = 0; v < accessor->count; ++v) {
                            float uv[2];
                            cgltf_accessor_read_float(accessor, v, uv, 2);
                            asset->mesh->vertices[baseIndex + v].uv = glm::vec3(uv[0], uv[1], 0.0f);
                        }
                    }
                }

                // Extract indices
                if (prim.indices) {
                    cgltf_accessor* indexAccessor = prim.indices;
                    size_t indexOffset = asset->mesh->indices.size();
                    asset->mesh->indices.resize(indexOffset + indexAccessor->count);

                    for (size_t i = 0; i < indexAccessor->count; ++i) {
                        unsigned int idx = 0;
                        cgltf_accessor_read_uint(indexAccessor, i, &idx, sizeof(cgltf_uint));
                        asset->mesh->indices[indexOffset + i] = idx + baseIndex; // adjust for combined mesh
                    }
                }

                // Extract textures from material
                if (prim.material) {
                    cgltf_material* mat = prim.material;

                    if (mat->has_pbr_metallic_roughness) {
                        cgltf_texture* tex = mat->pbr_metallic_roughness.base_color_texture.texture;

                        if (tex && tex->image) {
                            cgltf_image* img = tex->image;

                            std::vector<unsigned char> imageBytes;

                            // Extract raw bytes
                            if (img->buffer_view) {
                                cgltf_buffer_view* bv = img->buffer_view;
                                uint8_t* src = (uint8_t*)bv->buffer->data + bv->offset;
                                imageBytes.assign(src, src + bv->size);
                            }
                            else if (img->uri) {
                                std::cerr << "[GLBLoader] External texture URIs not supported: " << img->uri << std::endl;
                                continue;
                            }

                            // Decode PNG/JPG to RGBA using stb_image
                            int width, height, channels;
                            unsigned char* decoded = stbi_load_from_memory(
                                imageBytes.data(), static_cast<int>(imageBytes.size()),
                                &width, &height, &channels, 4
                            );

                            if (decoded) {
                                auto texture = std::make_shared<Texture>();
                                texture->width = width;
                                texture->height = height;
                                texture->channels = 4;
                                texture->data.assign(decoded, decoded + (width * height * 4));
                                stbi_image_free(decoded);

                                asset->textures.push_back(texture);
                                asset->mesh->textures.push_back(texture); // also link in mesh
                            } else {
                                std::cerr << "[GLBLoader] Failed to decode texture." << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }

    // ----------------------
    // Extract animations
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

                // Sample keyframes
                for (size_t k = 0; k < sampler->input->count; ++k) {
                    Keyframe key;
                    key.time = 0.0f;

                    // Read time
                    cgltf_accessor_read_float(sampler->input, k, &key.time, 1);

                    // Read value
                    if (channel->target_path == cgltf_animation_path_type_translation) {
                        float val[3];
                        cgltf_accessor_read_float(sampler->output, k, val, 3);
                        key.translation = glm::vec3(val[0], val[1], val[2]);
                    } 
                    else if (channel->target_path == cgltf_animation_path_type_rotation) {
                        float val[4];
                        cgltf_accessor_read_float(sampler->output, k, val, 4);
                        key.rotation = glm::quat(val[3], val[0], val[1], val[2]); // GLTF: xyzw
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

    // Assign to member
    asset_data = std::move(asset);

    if (data) {
        cgltf_free(data);
    }
}

GLBLoader::~GLBLoader() {
}
