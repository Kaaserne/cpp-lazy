#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ADAPTOR_HPP
#define LZ_CARTESIAN_PRODUCT_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/cartesian_product.hpp>

namespace lz {
namespace detail {
struct cartesian_product_adaptor {
    template<class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 cartesian_product_iterable<Iterables...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};

} // namespace detail
} // namespace lz
#endif
