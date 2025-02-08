#pragma once

#ifndef LZ_STRING_SPLITTER_HPP
#define LZ_STRING_SPLITTER_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/split.hpp>
#include <Lz/string_view.hpp>

namespace lz {
#ifdef LZ_HAS_CXX_11

static const detail::split_adaptor<void_t> split{};

static const detail::split_adaptor<std::basic_string> s_split{};

static const detail::split_adaptor<lz::basic_string_view> sv_split{};

#else

LZ_INLINE_VAR constexpr detail::split_adaptor<void> split{};

LZ_INLINE_VAR constexpr detail::split_adaptor<std::string> s_split{};

LZ_INLINE_VAR constexpr detail::split_adaptor<lz::string_view> sv_split{};

#endif

} // namespace lz

#endif // LZ_STRING_SPLITTER_HPP