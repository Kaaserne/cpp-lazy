#pragma once

#ifndef LZ_CONCATENATE_ADAPTOR_HPP
#define LZ_CONCATENATE_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/concatenate.hpp>

namespace lz {
namespace detail {
struct concatenate_adaptor {
    using adaptor = concatenate_adaptor;

    /**
     * @brief Concatenates multiple iterables into one iterable. The reference type returned by operator* will:
     * - be by value if one of the iterables yields by value
     * - be by const reference if one of the iterables yield by const reference.
     * - be by mutable reference if all iterables yield by mutable reference.
     * Contains a .size() function if all iterables have a .size() function. The size is the sum of all the sizes of the
     * iterables. Contains a sentinel if one of the iterables contains a sentinel or is forward. Its iterator category is the
     * 'weakest' of the input iterables. Example:
     * ```cpp
     * std::vector<int> a = {1, 2};
     * std::vector<int> b = {3, 4};
     * auto concatenated = lz::concat(a, b); // concatenated = {1, 2, 3, 4}
     * // or
     * auto concatenated = a | lz::concat(b); // concatenated = {1, 2, 3, 4}
     * auto range = lz::range(5, 10);
     * auto c = lz::concat(a, range); // yields by value, not by reference
     * ```
     * @param iterables The iterables to concatenate
     * @return An iterable that yields the concatenated elements of the input iterables.
     */
    template<class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 concatenate_iterable<remove_ref<Iterables>...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif
