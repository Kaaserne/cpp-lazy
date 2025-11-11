#pragma once

#ifndef LZ_EXCLUDE_ADAPTOR_HPP
#define LZ_EXCLUDE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/exclude.hpp>

namespace lz {
namespace detail {
struct exclude_adaptor {
    using adaptor = exclude_adaptor;

    /**
     * @brief Excludes elements from a container, using two indexes. The first index is means the start index, the second index
     * means the end index. Contains a .size() method if the input iterable contains a .size() method, its iterator category is
     * the same as the input iterable. Returns a sentinel if input iterable is forward or has a sentinel.
     *
     * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
     * traversed to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you
     * can use `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the
     * iterable. `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its
     * entire size is therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will
     * traverse the iterable once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again,
     * but will return the cached size instead. The following iterables require a(n) (eagerly)sized iterable:
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
     * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
     * // Exclude index [3, 6)
     * auto excluded = lz::exclude(vec, 3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
     * ```
     * @param iterable The iterable to exclude elements from.
     * @param from The start index to exclude.
     * @param to The end index to exclude.
     * @return An iterable that excludes the elements from the specified range.
     */
    template<class Iterable, class DiffType>
    LZ_NODISCARD constexpr exclude_iterable<remove_ref_t<Iterable>>
    operator()(Iterable&& iterable, DiffType from, DiffType to) const {
        return { std::forward<Iterable>(iterable), from, to };
    }

    /**
     * @brief Excludes elements from a container, using two indexes. The first index is means the start index, the second index
     * means the end index. Contains a .size() method if the input iterable contains a .size() method, its iterator category is
     * the same as the input iterable. Returns a sentinel if input iterable is forward or has a sentinel.
     *
     * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
     * traversed to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you
     * can use `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the
     * iterable. `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its
     * entire size is therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will
     * traverse the iterable once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again,
     * but will return the cached size instead. The following iterables require a(n) (eagerly)sized iterable:
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
     * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
     * // Exclude index [3, 6)
     * auto excluded = vec | lz::exclude(3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
     * ```
     * @param from The start index to exclude.
     * @param to The end index to exclude.
     * @return An adaptor that can be used in pipe expressions
     */
    template<class DiffType>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, DiffType, DiffType> operator()(DiffType from, DiffType to) const {
        return { from, to };
    }
};
} // namespace detail
} // namespace lz

#endif
