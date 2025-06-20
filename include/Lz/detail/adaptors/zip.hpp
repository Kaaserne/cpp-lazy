#pragma once

#ifndef LZ_ZIP_ADAPTOR_HPP
#define LZ_ZIP_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/zip.hpp>

namespace lz {
namespace detail {
struct zip_adaptor {
    using adaptor = zip_adaptor;

    /**
     * @brief Zips two or more iterables together. If the sizes of the iterables are different, the shortest one will be used. It
     * contains a size() method if all the iterables have a size() method. It is the same iterator category as the 'weakest' of
     * the input iterables. If the weakest is a forward iterator, then the end() function will return the same types as its input
     * iterables. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire
     * sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
     * So, all in all: use lz::cache_size if:
     * - Your iterable is exactly bidirectional (so forward/random access excluded) and
     * - Your iterable is not sized and
     * - You either use multiple/a combination of the following iterables OR (see last point):
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     * - Are planning to call begin() or end() multiple times on the same instance (with one or more of the above iterable
     * combinations) Example:
     * ```cpp
     * std::vector<int> a = { 1, 2, 3 };
     * std::vector<int> b = { 4, 5, 6 };
     * std::vector<int> c = { 7, 8, 9 };
     * auto zipped = lz::zip(a, b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
     *
     * // or
     *
     * auto zipped = a | lz::zip(b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
     *
     * std::vector<int> d = { 10 };
     * auto zipped2 = lz::zip(a, d); // { (1, 10) }
     * ```
     * @param iterables The iterables to zip together
     * @return A zip_iterable containing the zipped iterables
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_iterable<remove_ref<Iterables>...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_ADAPTOR_HPP
