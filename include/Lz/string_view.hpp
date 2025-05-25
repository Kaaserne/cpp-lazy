#pragma once

#ifndef LZ_STRING_VIEW_HPP
#define LZ_STRING_VIEW_HPP

#include <Lz/detail/compiler_checks.hpp>

#ifdef LZ_HAS_STRING_VIEW

#include <string_view>

#elif !defined(LZ_STANDALONE)

#include <fmt/core.h>

#else

#include <cstddef>

#endif

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#if defined(LZ_HAS_STRING_VIEW)

template<class C>
using basic_string_view = std::basic_string_view<C>;

using string_view = std::string_view;

#elif !defined(LZ_STANDALONE)

template<class C>
using basic_string_view = fmt::basic_string_view<C>;

using string_view = fmt::string_view;

#else

template<class CharT>
class basic_string_view {
public:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    using iterator = const CharT*;
    using const_iterator = const CharT*;
    using value_type = CharT;

    constexpr basic_string_view() = default;

    constexpr basic_string_view(const CharT* data, std::size_t size) noexcept : _data{ data }, _size{ size } {
    }

    LZ_CONSTEXPR_CXX_17 basic_string_view(const CharT* data) noexcept :
        _data{ data },
        _size{ std::char_traits<CharT>::length(data) } {
    }

    constexpr basic_string_view(const CharT* begin, const CharT* end) noexcept :
        _data{ begin },
        _size{ static_cast<std::size_t>(end - begin) } {
    }

    constexpr const CharT* data() const noexcept {
        return _data;
    }

    constexpr std::size_t size() const noexcept {
        return _size;
    }

    constexpr const CharT* begin() const noexcept {
        return _data;
    }

    constexpr const CharT* end() const noexcept {
        return _data + _size;
    }

    constexpr std::size_t length() const noexcept {
        return size();
    }

    constexpr bool empty() const noexcept {
        return _size == 0;
    }

    std::basic_string<CharT> to_std_string() const {
        return { _data, _size };
    }

    explicit operator std::basic_string<CharT>() const {
        return to_std_string();
    }

private:
    const CharT* _data{};
    std::size_t _size{};
};

#if !defined(LZ_HAS_CXX_17)

template<class CharT>
constexpr std::size_t basic_string_view<CharT>::npos;

#endif

using string_view = basic_string_view<char>;

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif
