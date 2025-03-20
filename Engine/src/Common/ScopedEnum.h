#pragma once

#include <type_traits>

namespace Engine
{
    template <typename T> struct has_flags : std::false_type {};
    template <typename T> constexpr auto has_flags_v = has_flags<T>::value;

    template <typename T>
    concept ScopedEnum = std::is_enum_v<T> && has_flags_v<T>;

    template <ScopedEnum T> [[nodiscard]] constexpr auto operator~(T a) {
        return static_cast<T>(~static_cast<std::underlying_type_t<T>>(a));
    }

    template <ScopedEnum T>
    [[nodiscard]] constexpr auto operator|(T lhs, T rhs) {
        return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) |
            static_cast<std::underlying_type_t<T>>(rhs));
    }
    template <ScopedEnum T> inline T& operator|=(T& lhs, T rhs) {
        return lhs = lhs | rhs;
    }

    template <ScopedEnum T>
    [[nodiscard]] constexpr auto operator&(T lhs, T rhs) {
        return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) &
            static_cast<std::underlying_type_t<T>>(rhs));
    }
    template <ScopedEnum T> inline T& operator&=(T& lhs, T rhs) {
        return lhs = lhs & rhs;
    }
}