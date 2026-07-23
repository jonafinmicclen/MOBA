#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <istream>


namespace BinaryLoader {
    struct BinaryData{
        std::array<std::uint8_t, 16> header;
        std::vector<std::uint8_t> data;
    };


    inline BinaryData loadBinary(std::istream& stream) {
        BinaryData result{};

        // Read first 16 bytes into header
        stream.read(
            reinterpret_cast<char*>(result.header.data()),
            static_cast<std::streamsize>(result.header.size())
        );

        if (stream.gcount() != static_cast<std::streamsize>(result.header.size())) {
            throw std::runtime_error("Binary stream is smaller than 16 bytes");
        }

        // Read the rest into a temporary char vector
        std::vector<char> rest{
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()
        };

        // Convert char bytes to uint8_t bytes
        result.data.assign(rest.begin(), rest.end());

        return result;
    }
};