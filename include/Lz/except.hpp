#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/except.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

detail::except_adaptor except{};

#else

LZ_INLINE_VAR constexpr detail::except_adaptor except{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_EXCEPT_HPP