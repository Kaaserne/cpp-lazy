#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_HPP
#define LZ_CARTESIAN_PRODUCT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/cartesian_product.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::cartesian_product_adaptor cartesian_product{};

#else

// TODO docs
/**
 * @brief
 *
 */
LZ_INLINE_VAR constexpr detail::cartesian_product_adaptor cartesian_product{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_HPP
