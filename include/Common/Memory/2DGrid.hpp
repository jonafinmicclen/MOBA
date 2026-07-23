#pragma once

#include <vector>


template<typename T>
class Grid2D {
public:
    // Some way to store scores for however many nodes
    Grid2D(const int width, const int height, const T default_value)
        : width_(width),
        height_(height),
        default_(default_value),
        data_(width * height, default_value) {
    }

    T& at(const int x, const int y) {
        return data_[width_*y + x];
    }

    const T& at(const int x, const int y) const {
        return data_[width_*y + x];
    }

private:
    std::vector<T> data_;
    const int width_;
    const int height_;
    const T default_;
};