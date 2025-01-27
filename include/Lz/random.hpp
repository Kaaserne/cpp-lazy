#pragma once

#ifndef LZ_RANDOM_HPP
#define LZ_RANDOM_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/random.hpp>
#include <random>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

detail::random_adaptor<false> random;

detail::random_adaptor<true> common_random;

#else

LZ_INLINE_VAR constexpr detail::random_adaptor<true> random{};

LZ_INLINE_VAR constexpr detail::random_adaptor<false> common_random{};

#endif
} // namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif