#pragma once

#ifndef LZ_TAKE_ADAPTOR_HPP
#define LZ_TAKE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/take.hpp>

namespace lz {
namespace detail {
struct take_adaptor {
    using adaptor = take_adaptor;

    /**
     * @brief This adaptor is used to take the first n elements of an iterable or iterator. The iterator category is the same as
     * the input iterator category. Has a .size() method that essentially returns
     * `n` or min(n, size(input_iterable)).
     *
     * If the parameter @p iter is an iterable:
     * - returns a default_sentinel_t if the input iterable is not bidirectional or has a sentinel
     * - if `n` is larger than `lz::eager_size(input_iterable)` then the total iterable size is equal to
     *   `lz::eager_size(input_iterable)`, thus preventing out of bounds. Traverses the entire iterable while taking the first `n`
     *   for non random access iterables.
     * - If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
     *   traversed to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether,
     *   you can use `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the
     *   iterable. `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its
     *   entire size is therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will
     *   traverse the iterable once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again,
     *   but will return the cached size instead. The following iterables require a(n) (eagerly)sized iterable:
     *   - `lz::chunks`
     *   - `lz::enumerate`
     *   - `lz::exclude`
     *   - `lz::interleave`
     *   - `lz::rotate`
     *   - `lz::take`
     *   - `lz::take_every`
     *   - `lz::zip_longest`
     *   - `lz::zip`
     *
     * If the parameter @p iter is an iterator:
     * - if `n` is larger than the actual size if the iterator then this is undefined behaviour.
     * - `lz::eager_size` is not used, so it is not guaranteed that the iterator will not go out of bounds.
     * - never returns a sentinel
     *
     * Example:
     * ```cpp
     * auto vec = std::vector<int>{1, 2, 3, 4, 5};
     * auto res = lz::take(vec, 2); // res = {1, 2}
     * // or
     * auto res = vec | lz::take(2); // res = {1, 2}
     * // or
     * auto res = lz::take(vec, 20); // res = {1, 2, 3, 4, 5}
     * // or
     * auto res = vec | lz::take(20); // res = {1, 2, 3, 4, 5}
     * auto res = lz::take(vec.begin(), 2); // res = {1, 2}
     * // auto res = lz::take(vec.begin(), 20); // undefined behaviour, as the iterator will go out of bounds
     * // vec.begin() | lz::take(2); // Not supported, as piping only works for iterables, not for iterators
     * ```
     * @param iter The iterable / iterator to take the first n elements from.
     * @param n The amount of elements to take.
     */
    template<class Iterable>
    LZ_NODISCARD constexpr take_iterable<remove_ref<Iterable>> operator()(Iterable&& iter, const std::size_t n) const {
        return { std::forward<Iterable>(iter), n };
    }

    /**
     * @brief This adaptor is used to take the first n elements of an iterable or iterator. The iterator category is the same as
     * the input iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. Has a
     * .size() method that essentially returns `n`. If `n` is larger than `lz::eager_size(input_iterable)` then
     * `lz::eager_size(input_iterable)` is used, thus preventing out of bounds.
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
     * auto vec = std::vector<int>{1, 2, 3, 4, 5};
     * auto res = lz::take(vec, 2); // res = {1, 2}
     * // or
     * auto res = vec | lz::take(2); // res = {1, 2}
     * // or
     * auto res = lz::take(vec, 20); // res = {1, 2, 3, 4, 5}
     * // or
     * auto res = vec | lz::take(20); // res = {1, 2, 3, 4, 5}
     * ```
     * @param n The amount of elements to take. Piping only works for iterables, not for iterators such as
     * `std::vector<int>::iterator`.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::size_t> operator()(const std::size_t n) const {
        return { n };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_ADAPTOR_HPP