#pragma once

#ifndef LZ_SLICE_HPP
#define LZ_SLICE_HPP

#include <Lz/drop.hpp>
#include <Lz/take.hpp>

namespace lz {
struct slice_adaptor {
#ifdef LZ_HAS_CXX_11
    static slice_adaptor slice;
#endif

    using adaptor = slice_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 detail::take_iterable<detail::drop_iterable<Iterable>>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> from, const diff_iterable_t<Iterable> to) const {
        return { lz::drop(std::forward<Iterable>(iterable), from), to - from };
    }

    template<class DiffT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::fn_args_holder<slice_adaptor, DiffT, DiffT>
    operator()(const DiffT from, const DiffT to) const {
        return { from, to };
    }
};

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to slice an iterable from `from` to `to`. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
 * iterable has a .size() method, then this iterable will also have a .size() method.Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
 * auto slice = lz::slice(vec, 2, 6); // slice = { 3, 4, 5, 6 }
 * // or
 * auto slice = vec | lz::slice(2, 6); // slice = { 3, 4, 5, 6 }
 * ```
 */
slice_adaptor slice_adaptor::slice{};

#else

/**
 * @brief This adaptor is used to slice an iterable from `from` to `to`. The iterator category is the same as the input
 * iterator category. Its end() function will return the same iterator as its input iterable. If its input iterable has a .size()
 * method, then this iterable will also have a .size() method.Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
 * auto slice = lz::slice(vec, 2, 6); // slice = { 3, 4, 5, 6 }
 * // or
 * auto slice = vec | lz::slice(2, 6); // slice = { 3, 4, 5, 6 }
 * ```
 */
LZ_INLINE_VAR constexpr slice_adaptor slice{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SLICE_HPP