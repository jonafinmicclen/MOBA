#pragma once

#include <cstdint>
#include <vector>
#include <istream>

#include "Assets/BinaryLoader.hpp"

using Chunk = std::uint8_t;
using Chunks = std::vector<Chunk>;

class WalkableMap {
public:
    WalkableMap(Chunks&& chunks, int width) :
        chunks_(std::move(chunks)), 
        width_(width),
        height_(chunks.size()/width) {}

    bool at(int x, int y) const {
        /**
         * Extracts the value at coords x, y.
         * 
         * First converts x, y to a chunk coord then extracts the value from the chunk
         */
        int chunk_x = x / 8;
        int bit_x = x % 8;
        int chunks_per_row = width_ / 8;

        Chunk mask = static_cast<Chunk>(Chunk{1} << (7 - bit_x));
        Chunk chunk = chunks_[chunk_x + y * chunks_per_row];

        return (chunk & mask) != 0;
    }

private:
    Chunks chunks_;
    int width_;
    int height_;
};


class WalkableMapLoader {
/**
 * 1 bit per pixel bitmap loader
 */


public:
    static inline WalkableMap load(std::istream& input) {
        auto binary = BinaryLoader::loadBinary(input);

        constexpr std::string_view MAGIC = "WALKABLEMAP";

        const bool magicMatches = std::equal(
            MAGIC.begin(),
            MAGIC.end(),
            binary.header.begin()
        );

        if (!magicMatches) {
            throw std::runtime_error("Invalid WalkableMap file: bad magic header");
        }

        return WalkableMap(std::move(binary.data), 512);
    }


private:
};