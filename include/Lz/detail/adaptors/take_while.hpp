#pragma once

#ifndef LZ_TAKE_WHILE_ADAPTOR
#define LZ_TAKE_WHILE_ADAPTOR


#include <Lz/detail/iterables/take_while.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>

namespace lz {
namespace detail {
struct take_while_adaptor {
    using adaptor = take_while_adaptor;

    /**
     * @brief Takes elements from an iterable while the given predicte returns `true`. Will return a bidirectional iterable if the
     * given iterable is at least bidirectional and doesn't have a sentinel. Does not contain a size() method and if its input
     * iterable is forward, will return a default_sentinel_t. Example:
     * ```cpp
     * std::vector<int> vec = {1, 2, 3, 4, 5};
     * auto take_while = lz::take_while(vec, [](int i) { return i < 3; }); // {1, 2}
     * ```
     * @param iterable The iterable to take elements from.
     * @param unary_predicate The predicate that indicates while to take elements.
     * @return A take_while_iterable that will yield elements from the input iterable while the predicate returns true.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr take_while_iterable<remove_ref<Iterable>, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate unary_predicate) const {
        return { std::forward<Iterable>(iterable), std::move(unary_predicate) };
    }

    /**
     * @brief Takes elements from an iterable while the given predicte returns `true`. Will return a bidirectional iterable if the
     * given iterable is at least bidirectional and doesn't have a sentinel. Does not contain a size() method and if its input
     * iterable is forward, will return a default_sentinel_t. Example:
     * ```cpp
     * std::vector<int> vec = {1, 2, 3, 4, 5};
     * auto take_while = vec | lz::take_while([](int i) { return i < 3; }); // {1, 2}
     * ```
     * @param unary_predicate The predicate that indicates while to take elements.
     * @return An adaptor that can be used in pipe expressions
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate unary_predicate) const {
        return { std::move(unary_predicate) };
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_TAKE_WHILE_ADAPTOR