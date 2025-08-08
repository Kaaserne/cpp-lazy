#pragma once

#ifndef LZ_FILTER_ADAPTOR_HPP
#define LZ_FILTER_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/filter.hpp>

namespace lz {
namespace detail {
struct filter_adaptor {
    using adaptor = filter_adaptor;

    /**
     * @brief Drops elements based on a condition predicate. If it returns `false`, the element in question is dropped. If it
     * returns `true` then this item is will be yielded. If the input iterable is forward an end() sentinel is returned, otherwise
     * the end() iterator is the same as the begin() iterator. It is bidirectional if the input iterable is also at least
     * bidirectional. It does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto filtered = lz::filter(vec, [](int i) { return i % 2 == 0; }); // { 2, 4 }
     * ```
     * @param iterable The iterable to filter
     * @param predicate The predicate to filter on
     * @return A filter_iterable that will yield the filtered elements
     */
    template<class Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr filter_iterable<remove_ref<Iterable>, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate predicate) const {
        return { std::forward<Iterable>(iterable), std::move(predicate) };
    }

    /**
     * @brief Drops elements based on a condition predicate. If it returns `false`, the element in question is dropped. If it
     * returns `true` then this item is will be yielded. If the input iterable is forward an end() sentinel is returned, otherwise
     * the end() iterator is the same as the begin() iterator. It is bidirectional if the input iterable is also at least
     * bidirectional. It does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto filtered = vec | lz::filter([](int i) { return i % 2 == 0; }); // { 2, 4 }
     * ```
     * @param predicate The predicate to filter on
     * @return An adaptor that can be used in pipe expressions
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate predicate) const {
        return { std::move(predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif
