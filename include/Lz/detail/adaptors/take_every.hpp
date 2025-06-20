#pragma once

#ifndef LZ_TAKE_EVERY_ADAPTOR_HPP
#define LZ_TAKE_EVERY_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/take_every.hpp>

namespace lz {
namespace detail {
struct take_every_adaptor {
    using adaptor = take_every_adaptor;

    /**
     * @brief Takes every `offset` element from the iterable, starting from `start`. Returns the same iterator category as the
     * input iterable. Contains a size() method if the input iterable is sized. If input iterable is forward or less, the end()
     * method will return a default sentinel. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for
     * example), the entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to
     * use `lz::cache_size`. So, all in all: use lz::cache_size if:
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
     * std::vector<int> vec = { 1, 2, 3, 4 };
     * auto take_every = lz::take_every(vec, 2); // 1, 3
     * auto take_every = lz::take_every(vec, 2, 1); // 2, 4
     * ```
     * @param iterable The iterable to take every `offset` element from
     * @param offset The offset to take every element from
     * @param start The start offset
     * @return A take_every_iterable that can be used to iterate over the taken elements.
     */
    template<class Iterable>
    LZ_NODISCARD constexpr enable_if<is_iterable<Iterable>::value, take_every_iterable<remove_ref<Iterable>>>
    operator()(Iterable&& iterable, const std::size_t offset, const std::size_t start = 0) const {
        return { std::forward<Iterable>(iterable), offset, start };
    }

    /**
     * @brief Takes every `offset` element from the iterable, starting from `start`. Returns the same iterator category as the
     * input iterable. Contains a size() method if the input iterable is sized. If input iterable is forward or less, the end()
     * method will return a default sentinel. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for
     * example), the entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to
     * use `lz::cache_size`. So, all in all: use lz::cache_size if:
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
     * std::vector<int> vec = { 1, 2, 3, 4 };
     * auto take_every = vec | lz::take_every(2); // 1, 3
     * auto take_every = vec | lz::take_every(2, 1); // 2, 4
     * ```
     * @param offset The offset to take every element from
     * @param start The start offset
     * @return An adaptor that can be used in pipe expressions
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::size_t, std::size_t>
    operator()(const std::size_t offset, const std::size_t start = 0) const {
        return { offset, start };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_EVERY_ADAPTOR_HPP