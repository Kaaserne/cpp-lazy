#pragma once

#ifndef LZ_DROP_WHILE_ADAPTOR_HPP
#define LZ_DROP_WHILE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/drop_while.hpp>

namespace lz {
namespace detail {
struct drop_while_adaptor {
    using adaptor = drop_while_adaptor;

    /**
     * @brief This adaptor is used to make an iterable where the iterator keeps dropping elements as long as the predicate returns
     * `true`. Once it has returned `false`, it will return the elements as usual. The iterator category is the same as its input
     * iterable. Its end() function will return a sentinel if its input iterable is forward or less or has a sentinel. If its
     * input iterable is random access, then this iterable will have a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto dropped = lz::drop_while(vec, [](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
     * ```
     * @param iterable The iterable to drop elements from
     * @param unary_predicate The predicate to drop elements with
     * @return An iterable that drops elements while the predicate returns true
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr drop_while_iterable<remove_ref<Iterable>, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate unary_predicate) const {
        return { std::forward<Iterable>(iterable), std::move(unary_predicate) };
    }

    /**
     * @brief This adaptor is used to make an iterable where the iterator keeps dropping elements as long as the predicate returns
     * `true`. Once it has returned `false`, it will return the elements as usual. The iterator category is the same as its input
     * iterable. Its end() function will return a sentinel if its input iterable is forward or less or has a sentinel. If its
     * input iterable is random access, then this iterable will have a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto dropped = vec | lz::drop_while([](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
     * ```
     * @param unary_predicate The predicate to drop elements with
     * @return An adaptor that can be used in a pipe expression
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate unary_predicate) const {
        return { std::move(unary_predicate) };
    }
};
} // namespace detail
} // namespace lz
#endif