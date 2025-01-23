#pragma once

#ifndef LZ_ENUMERATE_HPP
#define LZ_ENUMERATE_HPP

#include <Lz/detail/adaptors/enumerate.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

detail::enumerate_adaptor enumerate{};

#else

LZ_INLINE_VAR constexpr detail::enumerate_adaptor enumerate{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif