#pragma once

#ifndef LZ_DEFAULT_SENTINEL_HPP
#define LZ_DEFAULT_SENTINEL_HPP

#include <Lz/detail/compiler_config.hpp>

LZ_MODULE_EXPORT namespace lz {
struct default_sentinel_t {
    LZ_NODISCARD friend constexpr bool operator==(default_sentinel_t, default_sentinel_t) noexcept {
        return true;
    }

    LZ_NODISCARD friend constexpr bool operator!=(default_sentinel_t, default_sentinel_t) noexcept {
        return false;
    }

    LZ_NODISCARD friend constexpr bool operator<(default_sentinel_t, default_sentinel_t) noexcept {
        return false;
    }

    LZ_NODISCARD friend constexpr bool operator>(default_sentinel_t, default_sentinel_t) noexcept {
        return false;
    }

    LZ_NODISCARD friend constexpr bool operator>=(default_sentinel_t, default_sentinel_t) noexcept {
        return true;
    }

    LZ_NODISCARD friend constexpr bool operator<=(default_sentinel_t, default_sentinel_t) noexcept {
        return true;
    }
};

/**
 * @brief Holds a default sentinel value that can be used in iterators. Example:
 * ```cpp
 * auto it = lz::repeat(20, 5).begin();
 * if (it == lz::default_sentinel) {
 *     // do something
 * }
 * ```
 */
LZ_INLINE_VAR constexpr default_sentinel_t default_sentinel{};

} // namespace lz

#endif
