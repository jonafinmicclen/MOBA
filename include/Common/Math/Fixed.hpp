#pragma once

#include <cstdint>
#include <cmath>
#include <ostream>

// Fixed-point number with 3 decimal places of precision (i.e. X.XXX),
// backed by a 32-bit integer.
//
// This exists so that gameplay/world coordinates are deterministic across
// client/server and over the network, instead of drifting due to float
// rounding differences.
//
// Conversion in from float is implicit, so existing call sites that build
// coordinates from float literals/expressions keep compiling unchanged.
// Conversion back out to float is explicit (toFloat()) and is only meant
// to be used at rendering/output boundaries.
class Fixed {
public:
    static constexpr int32_t Scale = 1000;

    Fixed() = default;

    Fixed(float value)
        : raw_(static_cast<int32_t>(std::lround(value * static_cast<float>(Scale)))) {}

    [[nodiscard]] float toFloat() const {
        return static_cast<float>(raw_) / static_cast<float>(Scale);
    }

    [[nodiscard]] constexpr int32_t raw() const { return raw_; }

    static constexpr Fixed fromRaw(int32_t raw) {
        Fixed f;
        f.raw_ = raw;
        return f;
    }

    constexpr Fixed operator+(const Fixed& other) const { return fromRaw(raw_ + other.raw_); }
    constexpr Fixed operator-(const Fixed& other) const { return fromRaw(raw_ - other.raw_); }
    constexpr Fixed operator-() const { return fromRaw(-raw_); }

    constexpr Fixed& operator+=(const Fixed& other) { raw_ += other.raw_; return *this; }
    constexpr Fixed& operator-=(const Fixed& other) { raw_ -= other.raw_; return *this; }

    // Scalar float multiply/divide - used for things like speed scaling,
    // normalisation, and lerp factors where the other operand is a plain float.
    Fixed operator*(float scalar) const { return Fixed(toFloat() * scalar); }
    Fixed operator/(float scalar) const { return Fixed(toFloat() / scalar); }

    constexpr Fixed operator*(const Fixed& other) const {
        return fromRaw(static_cast<int32_t>((static_cast<int64_t>(raw_) * other.raw_) / Scale));
    }
    constexpr Fixed operator/(const Fixed& other) const {
        return fromRaw(static_cast<int32_t>((static_cast<int64_t>(raw_) * Scale) / other.raw_));
    }

    constexpr bool operator==(const Fixed& other) const { return raw_ == other.raw_; }
    constexpr bool operator!=(const Fixed& other) const { return raw_ != other.raw_; }
    constexpr bool operator<(const Fixed& other)  const { return raw_ < other.raw_; }
    constexpr bool operator>(const Fixed& other)  const { return raw_ > other.raw_; }
    constexpr bool operator<=(const Fixed& other) const { return raw_ <= other.raw_; }
    constexpr bool operator>=(const Fixed& other) const { return raw_ >= other.raw_; }

    friend std::ostream& operator<<(std::ostream& os, const Fixed& f) {
        return os << f.toFloat();
    }

private:
    int32_t raw_ = 0;
};

static_assert(sizeof(Fixed) == 4, "Fixed must stay 4 bytes to match existing wire format");
