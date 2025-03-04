#pragma once

#ifndef LZ_CONCATENATE_ADAPTOR_HPP
#define LZ_CONCATENATE_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/concatenate.hpp>

namespace lz {
namespace detail {
struct concatenate_adaptor {
    using adaptor = concatenate_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor concat{};

#endif

    /**
     * @brief Concatenates multiple iterables into one iterable. Contains a .size() function if all iterables have a .size()
     * function. The size is the sum of all the sizes of the iterables. Contains a sentinel if one of the iterables contains a
     * sentinel. Its iterator category is the 'weakest' of the input iterables. Example:
     * ```cpp
     * std::vector<int> a = {1, 2};
     * std::vector<int> b = {3, 4};
     * auto concatenated = lz::concat(a, b); // concatenated = {1, 2, 3, 4}
     * // or
     * auto concatenated = a | lz::concat(b); // concatenated = {1, 2, 3, 4}
     * ```
     * @param iterables The iterables to concatenate
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 concatenate_iterable<Iterables...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif
