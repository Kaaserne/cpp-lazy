#pragma once

#ifndef LZ_CACHED_SIZE_ADAPTOR_HPP
#define LZ_CACHED_SIZE_ADAPTOR_HPP

#include <Lz/detail/iterables/cached_size.hpp>

namespace lz {
namespace detail {
struct cached_size_adaptor {
    using adaptor = cached_size_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor cache_size;

#endif

    /**
     * @brief Creates an iterable with a size. Gets the size eagerly (using `lz::eager_size`), so if the input iterable is
     * bidirectional or less and is not sized, the entire iterable will be traversed. This can be handy if you want to
     * cache the size of an iterable. For instance, if you want to know the size multiple times. Also some lz iterables require an
     * (eager_)sized iterable. If you use multiple of these, it can be handy to cache the size. Please bear in mind that this is
     * only useful for non sized iterables and iterables that are bidirectional (so forward excluded). The following iterables
     * require a (eagerly)sized iterable:
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     *
     * ```cpp
     * auto to_filter = lz::range(10); // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
     * auto filtered = to_filter | lz::filter([](int i) { return i % 2 == 0; }); // filter isn't sized and is bidirectional
     *
     * // Get the size first, then chunks and enumerate will use the cached size
     * auto iterable = filtered | lz::cache_size | lz::chunks(3) | lz::enumerate;
     *
     * // iterable = { {0, {0, 2, 4}}, {1, {6, 8}} }
     * ```
     * @param iterable The iterable to cache the size of
     */
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 cached_size_iterable<Iterable> operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }
};
} // namespace detail
} // namespace lz

#endif
