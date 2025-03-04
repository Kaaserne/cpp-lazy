#pragma once

#ifndef LZ_ZIP_LONGEST_ADAPTOR_HPP
#define LZ_ZIP_LONGEST_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/zip_longest.hpp>

namespace lz {
namespace detail {
struct zip_longest_adaptor {
    using adaptor = zip_longest_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor zip_longest{};

#endif

    /**
     * @brief Zips multiple iterables together. If one of the iterables is shorter than the others, it will return an empty
     * optional instead of a non empty optional. The optional contains a tuple of `std::reference_wrapper`s to the elements itself
     * if it is not empty. Contains a size() method if all the iterables are sized. Will return the size of the largest iterable.
     * Its iterator category is the same as the iterator category of the 'weakest' iterator. Its end() function will return a
     * default_sentinel if the iterator is not bidirectional. Example:
     * ```cpp
     * std::vector<int> a = { 1, 2, 3 };
     * std::vector<int> b = { 4, 5 };
     * auto zipped = lz::zip_longest(a, b); // {(1, 4), (2, 5), (3, lz::nullopt)}
     * // or
     * auto zipped = a | lz::zip_longest(b); // {(1, 4), (2, 5), (3, lz::nullopt)}
     * ```
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_longest_iterable<Iterables...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif
