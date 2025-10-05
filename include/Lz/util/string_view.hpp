#pragma once

#ifndef LZ_STRING_VIEW_HPP
#define LZ_STRING_VIEW_HPP

#include <Lz/detail/compiler_checks.hpp>

#ifdef LZ_HAS_STRING_VIEW

#include <string_view>

#elif !defined(LZ_STANDALONE)

// clang-format off
#if !defined(LZ_STANDALONE)
  #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
  #endif
    #include <fmt/core.h>
  #ifdef __GNUC__
    #pragma GCC diagnostic pop
  #endif
#endif
// clang-format on

#endif

LZ_MODULE_EXPORT namespace lz {

#if defined(LZ_HAS_STRING_VIEW)

/**
 * @brief String view. Will use fmt, std or custom implementation depending on the build configuration.
 */
template<class C>
using basic_string_view = std::basic_string_view<C>;

/**
 * @brief String view. Will use fmt, std or custom implementation depending on the build configuration.
 */
using string_view = std::string_view;

#elif !defined(LZ_STANDALONE)

/**
 * @brief String view. Will use fmt, std or custom implementation depending on the build configuration.
 */
template<class C>
using basic_string_view = fmt::basic_string_view<C>;

/**
 * @brief String view. Will use fmt, std or custom implementation depending on the build configuration.
 */
using string_view = fmt::string_view;

#else

namespace detail {
template<class CharT>
constexpr size_t constexpr_str_len(const CharT* str, size_t n = 0) noexcept {
    return str[n] == static_cast<CharT>(0) ? n : constexpr_str_len(str, n + 1);
}
} // namespace detail

template<class CharT>
class basic_string_view {

public:
    using iterator = const CharT*;
    using const_iterator = const CharT*;
    using value_type = CharT;

    constexpr basic_string_view() = default;

    constexpr basic_string_view(const CharT* data, size_t size) noexcept : _data{ data }, _size{ size } {
    }

    constexpr basic_string_view(const CharT* data) noexcept : basic_string_view(data, detail::constexpr_str_len(data)) {
    }

    constexpr basic_string_view(const CharT* begin, const CharT* end) noexcept :
        _data{ begin },
        _size{ static_cast<size_t>(end - begin) } {
    }

    constexpr const CharT* data() const noexcept {
        return _data;
    }

    constexpr size_t size() const noexcept {
        return _size;
    }

    constexpr const CharT* begin() const noexcept {
        return _data;
    }

    constexpr const CharT* end() const noexcept {
        return _data == nullptr ? nullptr : _data + _size;
    }

    constexpr size_t length() const noexcept {
        return size();
    }

    constexpr bool empty() const noexcept {
        return _size == 0;
    }

private:
    const CharT* _data{};
    size_t _size{};
};

/**
 * @brief String view. Will use fmt, std or custom implementation depending on the build configuration.
 */
using string_view = basic_string_view<char>;

#endif

} // namespace lz

#endif
