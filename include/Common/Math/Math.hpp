#pragma once

#include <concepts>

namespace Math {

    template<typename P, typename U>
    concept Lerpable = requires(P a, P b, U t) {
        { a + ((b - a) * t) } -> std::convertible_to<P>;
    };

    template<typename P, typename U>
        requires Lerpable<P, U>
    [[nodiscard]] constexpr P lerp(P a, P b, U t) {
        return a + ((b - a) * t);
    }

}