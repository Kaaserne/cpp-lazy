#pragma once

#ifndef LZ_CHUNK_IF_ADAPTOR_HPP
#define LZ_CHUNK_IF_ADAPTOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/chunk_if.hpp>

namespace lz {
namespace detail {
template<class ValueType>
struct chunk_if_adaptor {
    using adaptor = chunk_if_adaptor<ValueType>;

    /**
     * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The
     * iterator category is forward and returns a sentinel. It returns an iterable of some type T of which T must be
     * constructible from its input begin() and end(). So essentially: `T(input_iterable.begin(), input_iterable.end())`. Example:
     * ```cpp
     * template<class T>
     * struct my_container {
     *   template<class I, class S>
     *   my_container(I begin, S end) {
     *      // stuff
     *   }
     * };
     * std::string s = "hello;world;;";
     * lz::chunk_if_adaptor<my_container<char>> my_chunk_if_fn;
     * auto chunked = my_chunk_if_fn(s, [](char c) { return c == ';'; });
     * // chunked = { my_container<char>{"hello"}, my_container<char>{"world"}, my_container<char>{""}, my_container<char>{""} }
     * ```
     * @param iterable The iterable to chunk.
     * @param predicate The predicate to chunk on.
     * @return An iterable of type T, where T is constructible from the begin and end iterators of the input iterable.
     */
    template<class Iterable, class UnaryPredicate>
    constexpr chunk_if_iterable<ValueType, remove_ref<Iterable>, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate predicate) const {
        return { std::forward<Iterable>(iterable), std::move(predicate) };
    }

    /**
     * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The
     * iterator category is forward and returns a sentinel. It returns an iterable of some type T of which T must be
     * constructible from its input begin() and end(). So essentially: `T(input_iterable.begin(), input_iterable.end())`. Example:
     * ```cpp
     * template<class T>
     * struct my_container {
     *   template<class I, class S>
     *   my_container(I begin, S end) {
     *      // stuff
     *   }
     * };
     * std::string s = "hello;world;;";
     * lz::chunk_if_adaptor<my_container<char>> my_chunk_if_fn;
     * auto chunked = s | my_chunk_if_fn([](char c) { return c == ';'; });
     * // chunked = { my_container<char>{"hello"}, my_container<char>{"world"}, my_container<char>{""}, my_container<char>{""} }
     * ```
     * @param predicate The predicate to chunk on.
     * @return An adaptor that can be used with the pipe operator to chunk the iterable.
     */
    template<class UnaryPredicate>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate predicate) const {
        return { std::move(predicate) };
    }
};

template<>
struct chunk_if_adaptor<void> {
    using adaptor = chunk_if_adaptor<void>;

    /**
     * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The
     * iterator category is forward and returns a sentinel. It returns an iterable of iterables. This iterable does not contain
     * a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} } }
     * ```
     * @param iterable The iterable to chunk.
     * @param predicate The predicate to chunk on.
     * @return An iterable of iterables, where each inner iterable is a chunk based on the predicate.
     */
    template<class Iterable, class UnaryPredicate>
    constexpr chunk_if_iterable<basic_iterable<iter_t<Iterable>, iter_t<Iterable>>, remove_ref<Iterable>, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate predicate) const {
        return { std::move(iterable), std::move(predicate) };
    }

    /**
     * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The
     * iterator category is forward and returns a sentinel. It returns an iterable of iterables. This iterable does not contain a
     * .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto chunked = vec | lz::chunk_if([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} } }
     * ```
     * @param predicate The predicate to chunk on.
     * @return An adaptor that can be used with the pipe operator to chunk the iterable.
     */
    template<class UnaryPredicate>
    constexpr fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate&& predicate) const {
        return { std::move(predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif