#pragma once

#ifndef LZ_RANGE_HPP
#define LZ_RANGE_HPP

#include <Lz/detail/adaptors/range.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

detail::range_adaptor detail::range_adaptor::range{};

#else

LZ_INLINE_VAR constexpr detail::range_adaptor range{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif