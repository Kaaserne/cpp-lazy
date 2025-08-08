#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ADAPTOR_HPP
#define LZ_CARTESIAN_PRODUCT_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/cartesian_product.hpp>

namespace lz {
namespace detail {
struct cartesian_product_adaptor {
    using adaptor = cartesian_product_adaptor;

    /**
     * @brief Performs a cartesian product on the given iterables. This means that it will return all possible combinations
     * of the elements of the given iterables. Contains a .size() function if all of the iterables have a .size() function.
     * Returns a sentinel if one of the iterables has a forward iterator or a sentinel. Example:
     * ```cpp
     * std::vector<int> a = {1, 2};
     * std::vector<int> b = {3, 4};
     * auto product = lz::cartesian_product(a, b); // product = {{1, 3}, {1, 4}, {2, 3}, {2, 4}}
     * // or
     * auto product = a | lz::cartesian_product(b); // product = {{1, 3}, {1, 4}, {2, 3}, {2, 4}}
     * ```
     * @param iterables The iterables to perform the cartesian product on.
     * @return A cartesian_product_iterable containing the cartesian product of the given iterables.
     */
    template<class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 cartesian_product_iterable<remove_ref<Iterables>...>
    operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};

} // namespace detail
} // namespace lz
#endif
