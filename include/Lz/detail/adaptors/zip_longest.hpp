#pragma once

#ifndef LZ_ZIP_LONGEST_ADAPTOR_HPP
#define LZ_ZIP_LONGEST_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/zip_longest.hpp>

namespace lz {
namespace detail {
struct zip_longest_adaptor {
    using adaptor = zip_longest_adaptor;

    /**
     * @brief Zips two or more iterables together. Returns a sentinel if one of the iterables has a sentinel or is forward. Its
     * value_type, a tuple, contains an optional of `std::reference_wrapper`s to the elements itself. If one of the iterables is
     * shorter than the others, it will return an empty optional instead of a non empty optional. Contains a size() method if all
     * the iterables are sized. Will return the size of the largest iterable. Its iterator category is the same as its 'weakest'
     * input iterables.
     *
     * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
     * traversed to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you
     * can use `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the
     * iterable. `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its
     * entire size is therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will
     * traverse the iterable once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again, but
     * will return the cached size instead. The following iterables require a(n) (eagerly)sized iterable:
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     *
     * Example:
     * ```cpp
     * std::vector<int> a = { 1, 2, 3 };
     * std::vector<int> b = { 4, 5 };
     * auto zipped = a | lz::zip_longest(b); // {(1, 4), (2, 5), (3, lz::nullopt)}
     *
     * auto filter = lz::filter(a, [](int i) { return i % 2 == 0; });
     * auto zipped = lz::zip_longest(a, filter); // {(1, lz::nullopt), (2, 2), (3, lz::nullopt)}
     * // zipped will be bidirectional. However, the entire sequence of `filter` will be traversed in order to get the size of the
     * // iterable. For `a` however, `a.size()` is used.
     * ```
     * @param iterables The iterables to zip together
     * @return A zip_longest_iterable containing the zipped iterables
     **/
    template<class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_longest_iterable<remove_ref<Iterables>...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif
