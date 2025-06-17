#pragma once

#ifndef LZ_CHUNKS_ADAPTOR_HPP
#define LZ_CHUNKS_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/chunks.hpp>

namespace lz {
namespace detail {

struct chunks_adaptor {
    using adaptor = chunks_adaptor;

    /**
     * @brief This adaptor is used to make chunks of the iterable, based on chunk size. The iterator
     * category is the same as its input iterable. It returns an iterable of iterables. Its end() function will return a sentinel,
     * if the input iterable has a forward iterator or has a sentinel. If its input iterable has a .size() method, then this
     * iterable will also have a .size() method. If the input iterable is exactly bidirectional and not sized (like `lz::filter`
     * for example), the entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while
     * to use `lz::cache_size`. So, all in all: use lz::cache_size if:
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
     *   combinations)
     *
     * Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto chunked = lz::chunks(vec, 3); // chunked = { {1, 2, 3}, {4, 5} }
     * // or
     * auto chunked = vec | lz::chunks(3); // chunked = { {1, 2, 3}, {4, 5} }
     * ```
     * @param iterable The iterable to chunk
     * @param chunk_size The size of the chunks
     * @return An iterable of iterables, where each inner iterable is a chunk of the original iterable
     **/
    template<LZ_CONCEPT_ITERABLE Iterable>
    constexpr chunks_iterable<remove_ref<Iterable>> operator()(Iterable&& iterable, std::size_t chunk_size) const {
        return { std::forward<Iterable>(iterable), chunk_size };
    }

    /**
     * @brief This adaptor is used to make chunks of the iterable, based on chunk size. The iterator
     * category is the same as its input iterable. It returns an iterable of iterables. Its end() function will return a sentinel,
     * if the input iterable has a forward iterator or has a sentinel. If its input iterable has a .size() method, then this
     *iterable will also have a .size() method. If the input iterable is exactly bidirectional and not sized (like `lz::filter`
     *for example), the entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while
     *to use `lz::cache_size`. So, all in all: use lz::cache_size if:
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
     *combinations)
     *
     * Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto chunked = vec | lz::chunks(3); // chunked = { {1, 2, 3}, {4, 5} }
     * ```
     * @param chunk_size The size of the chunks
     * @return An adaptor that can be used to chunk the iterable
     **/
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::size_t> operator()(std::size_t chunk_size) const {
        return { chunk_size };
    }
};

} // namespace detail
} // namespace lz

#endif