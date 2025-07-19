#pragma once

#ifndef LZ_CACHED_SIZE_ADAPTOR_HPP
#define LZ_CACHED_SIZE_ADAPTOR_HPP

#include <Lz/detail/iterables/cached_size.hpp>

namespace lz {
namespace detail {
struct cached_size_adaptor {
    using adaptor = cached_size_adaptor;

    /**
     * @brief Creates an iterable with a size method. Gets the size eagerly (using `lz::eager_size`). Some iterables will traverse
     * its entire input iterable to get to its end, or to get to know its size. For instance `z = zip(filter(x))` will traverse
     * the entire sequence of `filter` in `filter::end()` if `filter` is not sentinelled and if `filter` is bidirectional or
     * stronger. Now if you were to call `z.end()` it would traverse the entire `filter` iterable each time you
     * call `z.end()`. This is not very efficient, so you can use `lz::cache_size` to cache the size of the iterable. This will
     * traverse the iterable once and cache the size, so that subsequent calls to `z.end()` will not traverse the iterable again,
     * but will return the cached size instead. Another solution would be to use `lz::iter_decay`, defined in
     * `<Lz/iter_tools.hpp>`, to decay the iterable to a forward one.
     *
     * The following iterables require a(n) (eagerly)sized iterable:
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
     * auto to_filter = lz::range(10); // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
     * // f isn't sized and is bidirectional
     * auto f = to_filter | lz::filter([](int i) { return i % 2 == 0; }); // {0, 2, 4, 6, 8}
     *
     * // Get the size first, then chunks and enumerate will use the cached size
     * auto iterable = f | lz::cache_size | lz::chunks(3) | lz::enumerate;
     *
     * // iterable = { {0, {0, 2, 4}}, {1, {6, 8}} }
     * ```
     * @param iterable The iterable to cache the size of
     * @return A cached_size_iterable object that can be used to iterate over the elements in the iterable with a cached size.
     */
    template<class Iterable>
    LZ_NODISCARD constexpr cached_size_iterable<remove_ref<Iterable>> operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }
};
} // namespace detail
} // namespace lz

#endif
