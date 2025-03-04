#pragma once

#ifndef LZ_TAKE_ADAPTOR_HPP
#define LZ_TAKE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/take.hpp>

namespace lz {
namespace detail {
struct take_adaptor {
    using adaptor = take_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor take{};

#endif

    /**
     * @brief This adaptor is used to take the first n elements of an iterable. The iterator category is the same as the input
     * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. Has a .size()
     * method that essentially returns `n`. Example:
     * ```cpp
     * auto vec = std::vector<int>{1, 2, 3, 4, 5};
     * auto res = lz::take(vec, 2); // res = {1, 2}
     * ```
     * @param iterable The iterable to take the first n elements from.
     * @param n The amount of elements to take.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr take_iterable<Iterable> operator()(Iterable&& iterable, const diff_iterable_t<Iterable> n) const {
        return { std::forward<Iterable>(iterable), n };
    }

    /**
     * @brief This adaptor is used to take the first n elements of an iterable. The iterator category is the same as the input
     * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. Has a .size()
     * method that essentially returns `n`. Example:
     * ```cpp
     * auto vec = std::vector<int>{1, 2, 3, 4, 5};
     * auto res = vec | lz::take(2); // res = {1, 2}
     * ```
     * @param n The amount of elements to take.
     */
    template<class DiffT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, DiffT> operator()(const DiffT n) const {
        return { n };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_ADAPTOR_HPP