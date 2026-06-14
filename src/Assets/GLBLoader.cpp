#include "Assets/GLBLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define CGLTF_IMPLEMENTATION
#include "external/cgltf.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {


struct CgltfDeleter {
    void operator()(cgltf_data* data) const noexcept {
        if (data != nullptr) {
            cgltf_free(data);
        }
    }
};

using CgltfPtr =
    std::unique_ptr<cgltf_data, CgltfDeleter>;

bool canFitInUint32(
    const std::size_t baseVertex,
    const std::size_t vertexCount
) {
    constexpr std::size_t maxValue =
        std::numeric_limits<std::uint32_t>::max();

    return baseVertex <= maxValue &&
           vertexCount <= maxValue - baseVertex;
}

} // namespace

GLBLoader::GLBLoader(const char* path) {
    if (path == nullptr || path[0] == '\0') {
        std::cerr
            << "[GLBLoader] Empty asset path\n";

        return;
    }

    // ---------------------------------------------------------
    // Parse file
    // ---------------------------------------------------------

    cgltf_options options{};
    cgltf_data* rawData = nullptr;

    cgltf_result result =
        cgltf_parse_file(
            &options,
            path,
            &rawData
        );

    if (result != cgltf_result_success) {
        std::cerr
            << "[GLBLoader] Failed to parse GLB: "
            << static_cast<int>(result)
            << '\n';

        return;
    }

    CgltfPtr data{rawData};

    // ---------------------------------------------------------
    // Load binary buffers
    // ---------------------------------------------------------

    result = cgltf_load_buffers(
        &options,
        data.get(),
        path
    );

    if (result != cgltf_result_success) {
        std::cerr
            << "[GLBLoader] Failed to load buffers: "
            << static_cast<int>(result)
            << '\n';

        return;
    }

    // ---------------------------------------------------------
    // Validate GLB
    // ---------------------------------------------------------

    result = cgltf_validate(data.get());

    if (result != cgltf_result_success) {
        std::cerr
            << "[GLBLoader] GLB validation failed: "
            << static_cast<int>(result)
            << '\n';

        return;
    }

    Asset asset{};

    asset.textures.reserve(data->images_count);
    asset.animations.reserve(data->animations_count);

    /*
     * Multiple primitives may reference the same image.
     *
     * This cache ensures that each image is decoded and stored
     * only once in Asset::textures.
     */
    std::unordered_map<const cgltf_image*, int>
        textureIndices;

    textureIndices.reserve(data->images_count);

    auto getTextureIndex =
        [&](cgltf_image* image, int uvIndex) -> int {
            if (image == nullptr) {
                return -1;
            }

            const auto existing =
                textureIndices.find(image);

            if (existing != textureIndices.end()) {
                return existing->second;
            }

            std::optional<Texture> loadedTexture =
                LoadTexture(image);

            if (!loadedTexture) {
                return -1;
            }

            if (
                asset.textures.size() >
                static_cast<std::size_t>(
                    std::numeric_limits<int>::max()
                )
            ) {
                std::cerr
                    << "[GLBLoader] Too many textures\n";

                return -1;
            }

            loadedTexture->uvIndex = uvIndex;

            const int textureIndex =
                static_cast<int>(
                    asset.textures.size()
                );

            asset.textures.push_back(
                std::move(*loadedTexture)
            );

            textureIndices.emplace(
                image,
                textureIndex
            );

            return textureIndex;
        };

    // ---------------------------------------------------------
    // Count vertices, indices and faces for reserve()
    // ---------------------------------------------------------

    std::size_t totalVertexCount = 0;
    std::size_t totalIndexCount = 0;
    std::size_t totalFaceCount = 0;

    for (
        std::size_t meshIndex = 0;
        meshIndex < data->meshes_count;
        ++meshIndex
    ) {
        const cgltf_mesh& sourceMesh =
            data->meshes[meshIndex];

        for (
            std::size_t primitiveIndex = 0;
            primitiveIndex <
                sourceMesh.primitives_count;
            ++primitiveIndex
        ) {
            const cgltf_primitive& primitive =
                sourceMesh.primitives[
                    primitiveIndex
                ];

            if (
                primitive.type !=
                cgltf_primitive_type_triangles
            ) {
                continue;
            }

            std::size_t primitiveVertexCount = 0;

            for (
                std::size_t attributeIndex = 0;
                attributeIndex <
                    primitive.attributes_count;
                ++attributeIndex
            ) {
                const cgltf_attribute& attribute =
                    primitive.attributes[
                        attributeIndex
                    ];

                if (
                    attribute.type ==
                        cgltf_attribute_type_position &&
                    attribute.data != nullptr
                ) {
                    primitiveVertexCount =
                        attribute.data->count;

                    break;
                }
            }

            totalVertexCount +=
                primitiveVertexCount;

            if (primitive.indices != nullptr) {
                totalIndexCount +=
                    primitive.indices->count;

                totalFaceCount +=
                    primitive.indices->count / 3;
            } else {
                totalIndexCount +=
                    primitiveVertexCount;

                totalFaceCount +=
                    primitiveVertexCount / 3;
            }
        }
    }

    asset.mesh.vertices.reserve(
        totalVertexCount
    );

    asset.mesh.indices.reserve(
        totalIndexCount
    );

    asset.mesh.faceTextureIndices.reserve(
        totalFaceCount
    );

    // Scratch storage reused for vertex attributes.
    std::vector<float> attributeScratch;

    // ---------------------------------------------------------
    // Load meshes
    // ---------------------------------------------------------

    for (
        std::size_t meshIndex = 0;
        meshIndex < data->meshes_count;
        ++meshIndex
    ) {
        const cgltf_mesh& sourceMesh =
            data->meshes[meshIndex];

        for (
            std::size_t primitiveIndex = 0;
            primitiveIndex <
                sourceMesh.primitives_count;
            ++primitiveIndex
        ) {
            const cgltf_primitive& primitive =
                sourceMesh.primitives[
                    primitiveIndex
                ];

            if (
                primitive.type !=
                cgltf_primitive_type_triangles
            ) {
                std::cerr
                    << "[GLBLoader] Skipping "
                       "non-triangle primitive\n";

                continue;
            }

            // -------------------------------------------------
            // Find the primitive's albedo texture
            // -------------------------------------------------

            int textureIndex = -1;

            if (primitive.material != nullptr) {
                cgltf_material* material =
                    primitive.material;

                if (
                    material
                        ->has_pbr_metallic_roughness
                ) {
                    cgltf_texture_view& baseColor =
                        material
                            ->pbr_metallic_roughness
                            .base_color_texture;

                    if (
                        baseColor.texture != nullptr &&
                        baseColor.texture->image !=
                            nullptr
                    ) {
                        /*
                         * Vertex currently contains only one UV
                         * channel, so only TEXCOORD_0 is usable.
                         */
                        if (baseColor.texcoord != 0) {
                            std::cerr
                                << "[GLBLoader] Unsupported "
                                   "albedo UV set: "
                                << baseColor.texcoord
                                << '\n';
                        } else {
                            textureIndex =
                                getTextureIndex(
                                    baseColor
                                        .texture
                                        ->image,
                                    baseColor.texcoord
                                );
                        }
                    }
                }
            }

            // -------------------------------------------------
            // Find position accessor and vertex count
            // -------------------------------------------------

            const cgltf_accessor* positionAccessor =
                nullptr;

            for (
                std::size_t attributeIndex = 0;
                attributeIndex <
                    primitive.attributes_count;
                ++attributeIndex
            ) {
                const cgltf_attribute& attribute =
                    primitive.attributes[
                        attributeIndex
                    ];

                if (
                    attribute.type ==
                        cgltf_attribute_type_position &&
                    attribute.data != nullptr
                ) {
                    positionAccessor =
                        attribute.data;

                    break;
                }
            }

            if (positionAccessor == nullptr) {
                std::cerr
                    << "[GLBLoader] Primitive has no "
                       "POSITION attribute\n";

                continue;
            }

            const std::size_t vertexCount =
                positionAccessor->count;

            const std::size_t baseVertex =
                asset.mesh.vertices.size();

            if (
                !canFitInUint32(
                    baseVertex,
                    vertexCount
                )
            ) {
                std::cerr
                    << "[GLBLoader] Mesh exceeds "
                       "uint32_t index range\n";

                return;
            }

            asset.mesh.vertices.resize(
                baseVertex + vertexCount
            );

            // -------------------------------------------------
            // Load vertex attributes
            // -------------------------------------------------

            for (
                std::size_t attributeIndex = 0;
                attributeIndex <
                    primitive.attributes_count;
                ++attributeIndex
            ) {
                const cgltf_attribute& attribute =
                    primitive.attributes[
                        attributeIndex
                    ];

                const cgltf_accessor* accessor =
                    attribute.data;

                if (accessor == nullptr) {
                    continue;
                }

                if (
                    attribute.type ==
                    cgltf_attribute_type_position
                ) {
                    const std::size_t valueCount =
                        accessor->count * 3;

                    attributeScratch.resize(
                        valueCount
                    );

                    const cgltf_size unpacked =
                        cgltf_accessor_unpack_floats(
                            accessor,
                            attributeScratch.data(),
                            attributeScratch.size()
                        );

                    if (unpacked < valueCount) {
                        std::cerr
                            << "[GLBLoader] Could not "
                               "unpack positions\n";

                        return;
                    }

                    const std::size_t count =
                        std::min<std::size_t>(
                            accessor->count,
                            vertexCount
                        );

                    for (
                        std::size_t vertexIndex = 0;
                        vertexIndex < count;
                        ++vertexIndex
                    ) {
                        const std::size_t valueIndex =
                            vertexIndex * 3;

                        asset.mesh.vertices[
                            baseVertex + vertexIndex
                        ].position = glm::vec3{
                            attributeScratch[
                                valueIndex + 0
                            ],
                            attributeScratch[
                                valueIndex + 1
                            ],
                            attributeScratch[
                                valueIndex + 2
                            ]
                        };
                    }
                }
                else if (
                    attribute.type ==
                    cgltf_attribute_type_normal
                ) {
                    const std::size_t valueCount =
                        accessor->count * 3;

                    attributeScratch.resize(
                        valueCount
                    );

                    const cgltf_size unpacked =
                        cgltf_accessor_unpack_floats(
                            accessor,
                            attributeScratch.data(),
                            attributeScratch.size()
                        );

                    if (unpacked < valueCount) {
                        std::cerr
                            << "[GLBLoader] Could not "
                               "unpack normals\n";

                        return;
                    }

                    const std::size_t count =
                        std::min<std::size_t>(
                            accessor->count,
                            vertexCount
                        );

                    for (
                        std::size_t vertexIndex = 0;
                        vertexIndex < count;
                        ++vertexIndex
                    ) {
                        const std::size_t valueIndex =
                            vertexIndex * 3;

                        asset.mesh.vertices[
                            baseVertex + vertexIndex
                        ].normal = glm::vec3{
                            attributeScratch[
                                valueIndex + 0
                            ],
                            attributeScratch[
                                valueIndex + 1
                            ],
                            attributeScratch[
                                valueIndex + 2
                            ]
                        };
                    }
                }
                else if (
                    attribute.type ==
                        cgltf_attribute_type_texcoord &&
                    attribute.index == 0
                ) {
                    const std::size_t valueCount =
                        accessor->count * 2;

                    attributeScratch.resize(
                        valueCount
                    );

                    const cgltf_size unpacked =
                        cgltf_accessor_unpack_floats(
                            accessor,
                            attributeScratch.data(),
                            attributeScratch.size()
                        );

                    if (unpacked < valueCount) {
                        std::cerr
                            << "[GLBLoader] Could not "
                               "unpack UV coordinates\n";

                        return;
                    }

                    const std::size_t count =
                        std::min<std::size_t>(
                            accessor->count,
                            vertexCount
                        );

                    for (
                        std::size_t vertexIndex = 0;
                        vertexIndex < count;
                        ++vertexIndex
                    ) {
                        const std::size_t valueIndex =
                            vertexIndex * 2;

                        asset.mesh.vertices[
                            baseVertex + vertexIndex
                        ].uv = glm::vec2{
                            attributeScratch[
                                valueIndex + 0
                            ],
                            attributeScratch[
                                valueIndex + 1
                            ]
                        };
                    }
                }
            }

            // -------------------------------------------------
            // Load index data
            // -------------------------------------------------

            if (primitive.indices != nullptr) {
                const cgltf_accessor* indexAccessor =
                    primitive.indices;

                const std::size_t indexCount =
                    indexAccessor->count;

                for (
                    std::size_t indexNumber = 0;
                    indexNumber < indexCount;
                    ++indexNumber
                ) {
                    const cgltf_size localIndex =
                        cgltf_accessor_read_index(
                            indexAccessor,
                            indexNumber
                        );

                    if (localIndex >= vertexCount) {
                        std::cerr
                            << "[GLBLoader] Primitive "
                               "index is outside its "
                               "vertex range\n";

                        return;
                    }

                    const std::size_t globalIndex =
                        baseVertex + localIndex;

                    asset.mesh.indices.push_back(
                        static_cast<std::uint32_t>(
                            globalIndex
                        )
                    );
                }

                const std::size_t faceCount =
                    indexCount / 3;

                asset.mesh
                    .faceTextureIndices
                    .insert(
                        asset.mesh
                            .faceTextureIndices
                            .end(),
                        faceCount,
                        textureIndex
                    );
            } else {
                /*
                 * Generate sequential indices for
                 * non-indexed triangle primitives.
                 */
                for (
                    std::size_t vertexIndex = 0;
                    vertexIndex < vertexCount;
                    ++vertexIndex
                ) {
                    asset.mesh.indices.push_back(
                        static_cast<std::uint32_t>(
                            baseVertex + vertexIndex
                        )
                    );
                }

                const std::size_t faceCount =
                    vertexCount / 3;

                asset.mesh
                    .faceTextureIndices
                    .insert(
                        asset.mesh
                            .faceTextureIndices
                            .end(),
                        faceCount,
                        textureIndex
                    );
            }
        }
    }

    // ---------------------------------------------------------
    // Load animations
    // ---------------------------------------------------------

    for (
        std::size_t animationIndex = 0;
        animationIndex <
            data->animations_count;
        ++animationIndex
    ) {
        const cgltf_animation& sourceAnimation =
            data->animations[animationIndex];

        AnimationData animation{};

        animation.name =
            sourceAnimation.name != nullptr
                ? sourceAnimation.name
                : "Unnamed";

        animation.channels.reserve(
            sourceAnimation.channels_count
        );

        for (
            std::size_t channelIndex = 0;
            channelIndex <
                sourceAnimation.channels_count;
            ++channelIndex
        ) {
            const cgltf_animation_channel& channel =
                sourceAnimation.channels[
                    channelIndex
                ];

            const cgltf_animation_sampler* sampler =
                channel.sampler;

            if (
                sampler == nullptr ||
                sampler->input == nullptr ||
                sampler->output == nullptr
            ) {
                continue;
            }

            NodeAnimation nodeAnimation{};

            nodeAnimation.nodeName =
                channel.target_node != nullptr &&
                channel.target_node->name != nullptr
                    ? channel.target_node->name
                    : "UnnamedNode";

            const std::size_t keyframeCount =
                sampler->input->count;

            if (keyframeCount == 0) {
                continue;
            }

            std::vector<float> times(
                keyframeCount
            );

            const cgltf_size unpackedTimes =
                cgltf_accessor_unpack_floats(
                    sampler->input,
                    times.data(),
                    times.size()
                );

            if (
                unpackedTimes <
                keyframeCount
            ) {
                std::cerr
                    << "[GLBLoader] Could not unpack "
                       "animation times\n";

                continue;
            }

            std::size_t componentCount = 0;

            switch (channel.target_path) {
                case
                cgltf_animation_path_type_translation:

                    componentCount = 3;
                    break;

                case
                cgltf_animation_path_type_rotation:

                    componentCount = 4;
                    break;

                case
                cgltf_animation_path_type_scale:

                    componentCount = 3;
                    break;

                default:
                    /*
                     * Morph target animation is not
                     * supported by this loader.
                     */
                    continue;
            }

            std::vector<float> outputValues(
                keyframeCount * componentCount
            );

            const cgltf_size unpackedOutput =
                cgltf_accessor_unpack_floats(
                    sampler->output,
                    outputValues.data(),
                    outputValues.size()
                );

            if (
                unpackedOutput <
                keyframeCount * componentCount
            ) {
                std::cerr
                    << "[GLBLoader] Could not unpack "
                       "animation output\n";

                continue;
            }

            nodeAnimation.keyframes.reserve(
                keyframeCount
            );

            for (
                std::size_t keyframeIndex = 0;
                keyframeIndex < keyframeCount;
                ++keyframeIndex
            ) {
                Keyframe keyframe{};

                keyframe.time =
                    times[keyframeIndex];

                /*
                 * Each animation channel usually modifies
                 * only one transform property.
                 */
                keyframe.translation =
                    glm::vec3{0.0f};

                keyframe.rotation =
                    glm::quat{
                        1.0f,
                        0.0f,
                        0.0f,
                        0.0f
                    };

                keyframe.scale =
                    glm::vec3{1.0f};

                const std::size_t valueOffset =
                    keyframeIndex *
                    componentCount;

                switch (channel.target_path) {
                    case
                    cgltf_animation_path_type_translation:

                        keyframe.translation =
                            glm::vec3{
                                outputValues[
                                    valueOffset + 0
                                ],
                                outputValues[
                                    valueOffset + 1
                                ],
                                outputValues[
                                    valueOffset + 2
                                ]
                            };

                        break;

                    case
                    cgltf_animation_path_type_rotation:

                        /*
                         * glTF stores x, y, z, w.
                         * GLM expects w, x, y, z.
                         */
                        keyframe.rotation =
                            glm::quat{
                                outputValues[
                                    valueOffset + 3
                                ],
                                outputValues[
                                    valueOffset + 0
                                ],
                                outputValues[
                                    valueOffset + 1
                                ],
                                outputValues[
                                    valueOffset + 2
                                ]
                            };

                        break;

                    case
                    cgltf_animation_path_type_scale:

                        keyframe.scale =
                            glm::vec3{
                                outputValues[
                                    valueOffset + 0
                                ],
                                outputValues[
                                    valueOffset + 1
                                ],
                                outputValues[
                                    valueOffset + 2
                                ]
                            };

                        break;

                    default:
                        break;
                }

                nodeAnimation.keyframes.push_back(
                    std::move(keyframe)
                );
            }

            animation.channels.push_back(
                std::move(nodeAnimation)
            );
        }

        asset.animations.push_back(
            std::move(animation)
        );
    }

    // ---------------------------------------------------------
    // Move completed Asset into std::optional<Asset>
    // ---------------------------------------------------------

    asset_data = std::move(asset);
}

GLBLoader::~GLBLoader() = default;

std::optional<Texture>
GLBLoader::LoadTexture(cgltf_image* image) {
    if (image == nullptr) {
        return std::nullopt;
    }

    const std::uint8_t* encodedData = nullptr;
    std::size_t encodedSize = 0;

    if (image->buffer_view != nullptr) {
        const cgltf_buffer_view* bufferView =
            image->buffer_view;

        if (
            bufferView->buffer == nullptr ||
            bufferView->buffer->data == nullptr ||
            bufferView->size == 0
        ) {
            std::cerr
                << "[GLBLoader] Invalid embedded "
                   "image buffer\n";

            return std::nullopt;
        }

        const auto* bufferData =
            static_cast<const std::uint8_t*>(
                bufferView->buffer->data
            );

        encodedData =
            bufferData + bufferView->offset;

        encodedSize =
            static_cast<std::size_t>(
                bufferView->size
            );
    }
    else if (image->uri != nullptr) {
        std::cerr
            << "[GLBLoader] External texture URIs "
               "are not supported: "
            << image->uri
            << '\n';

        return std::nullopt;
    }
    else {
        std::cerr
            << "[GLBLoader] Image has no buffer "
               "view or URI\n";

        return std::nullopt;
    }

    if (
        encodedData == nullptr ||
        encodedSize == 0 ||
        encodedSize >
            static_cast<std::size_t>(
                std::numeric_limits<int>::max()
            )
    ) {
        std::cerr
            << "[GLBLoader] Invalid encoded "
               "image size\n";

        return std::nullopt;
    }

    int width = 0;
    int height = 0;
    int originalChannels = 0;

    unsigned char* rawPixels =
        stbi_load_from_memory(
            encodedData,
            static_cast<int>(encodedSize),
            &width,
            &height,
            &originalChannels,
            4
        );

    if (rawPixels == nullptr) {
        const char* failureReason =
            stbi_failure_reason();

        std::cerr
            << "[GLBLoader] Texture decode failed: "
            << (
                failureReason != nullptr
                    ? failureReason
                    : "unknown STB error"
            )
            << '\n';

        return std::nullopt;
    }

    const auto stbDeleter =
        [](unsigned char* pixels) noexcept {
            stbi_image_free(pixels);
        };

    std::unique_ptr<
        unsigned char,
        decltype(stbDeleter)
    > decodedPixels{
        rawPixels,
        stbDeleter
    };

    if (width <= 0 || height <= 0) {
        std::cerr
            << "[GLBLoader] Texture has invalid "
               "dimensions\n";

        return std::nullopt;
    }

    const std::size_t widthSize =
        static_cast<std::size_t>(width);

    const std::size_t heightSize =
        static_cast<std::size_t>(height);

    if (
        widthSize >
        std::numeric_limits<std::size_t>::max() /
            heightSize
    ) {
        std::cerr
            << "[GLBLoader] Texture size overflow\n";

        return std::nullopt;
    }

    const std::size_t pixelCount =
        widthSize * heightSize;

    if (
        pixelCount >
        std::numeric_limits<std::size_t>::max() /
            4
    ) {
        std::cerr
            << "[GLBLoader] Texture byte-size "
               "overflow\n";

        return std::nullopt;
    }

    const std::size_t byteCount =
        pixelCount * 4;

    Texture texture{};

    texture.width = width;
    texture.height = height;
    texture.channels = 4;
    texture.uvIndex = 0;

    texture.data.assign(
        decodedPixels.get(),
        decodedPixels.get() + byteCount
    );

    return texture;
}