#pragma once

#ifndef LZ_GENERATE_HPP
#define LZ_GENERATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/generate.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::generate_adaptor generate{};

#else

LZ_INLINE_VAR constexpr detail::generate_adaptor generate{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif