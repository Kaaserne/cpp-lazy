#pragma once

#ifndef LZ_INTERSECTION_HPP
#define LZ_INTERSECTION_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/interleave.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

// TODO docs

#ifdef LZ_HAS_CXX_11

constexpr detail::interleave_adaptor interleave{};

#else

LZ_INLINE_VAR constexpr detail::interleave_adaptor interleave{};

#endif

using detail::interleave_iterator;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif
