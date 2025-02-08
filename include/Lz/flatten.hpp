#ifndef LZ_FLATTEN_HPP
#define LZ_FLATTEN_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/flatten.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::flatten_adaptor flatten{};

#else

LZ_INLINE_VAR constexpr detail::flatten_adaptor flatten{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_FLATTEN_HPP
