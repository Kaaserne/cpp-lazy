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
     * Its iterator category is the same as its 'weakest' input iterables. Please keep in mind that:
     *   - If all of the input iterables are bidirectional or higher then:
     *     - The zip_longest iterator will be bidirectional
     *     - The zip_longest iterator will get the sizes from the iterables using the size() method, or `lz/std::distance` if that
     *       iterable does not provide a size method. Example:
     * ```cpp
     * std::vector<int> a = { 1, 2, 3 };
     * std::vector<int> b = { 4, 5 };
     * auto zipped = lz::zip_longest(a, b); // {(1, 4), (2, 5), (3, lz::nullopt)}
     * // or
     * auto zipped = a | lz::zip_longest(b); // {(1, 4), (2, 5), (3, lz::nullopt)}
     *
     * auto filter = lz::filter(a, [](int i) { return i % 2 == 0; });
     * auto zipped = lz::zip_longest(a, filter); // {(1, lz::nullopt), (2, 2), (3, lz::nullopt)}
     * // zipped will be bidirectional. However, the entire sequence of `filter` will be traversed in order to get the size of the
     * // iterable. For `a` however, `a.size()` is used.
     * ```
     * @param iterables The iterables to zip together
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_longest_iterable<Iterables...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif
