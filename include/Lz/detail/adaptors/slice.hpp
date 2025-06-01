#pragma once

#ifndef LZ_SLICE_ADAPTOR_HPP
#define LZ_SLICE_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/drop.hpp>
#include <Lz/take.hpp>

namespace lz {
namespace detail {
template<class Iterable>
using slice_iterable = detail::take_iterable<detail::drop_iterable<Iterable>>;

struct slice_adaptor {
    using adaptor = slice_adaptor;

    /**
     * @brief This adaptor is used to slice an iterable from `from` to `to`. The iterator category is the same as the input
     * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
     * iterable has a .size() method, then this iterable will also have a .size() method.Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
     * auto slice = lz::slice(vec, 2, 6); // slice = { 3, 4, 5, 6 }
     * ```
     * @param iterable The iterable to slice
     * @param from The start index of the slice
     * @param to The end index of the slice
     * @return A slice_iterable that can be used to iterate over the sliced elements.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 slice_iterable<remove_ref<Iterable>>
    operator()(Iterable&& iterable, const std::size_t from, const std::size_t to) const {
        LZ_ASSERT(to > from, "`to` must be greater than `from`");
        return { lz::drop(std::forward<Iterable>(iterable), from), to - from };
    }

    /**
     * @brief This adaptor is used to slice an iterable from `from` to `to`. The iterator category is the same as the input
     * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
     * iterable has a .size() method, then this iterable will also have a .size() method.Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
     * auto slice = vec | lz::slice(2, 6); // slice = { 3, 4, 5, 6 }
     * ```
     * @param from The start index of the slice
     * @param to The end index of the slice
     * @return An adaptor that can be used in pipe expressions
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::fn_args_holder<adaptor, std::size_t, std::size_t>
    operator()(const std::size_t from, const std::size_t to) const {
        return { from, to };
    }
};
} // namespace detail
} // namespace lz

#endif
