#pragma once

#ifndef LZ_CONCATENATE_HPP
#define LZ_CONCATENATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/concatenate.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11
#else

LZ_INLINE_VAR constexpr detail::concatenate_adaptor concat{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CONCATENATE_HPP