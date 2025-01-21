#pragma once

#ifndef LZ_DROP_HPP
#define LZ_DROP_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/drop.hpp>

namespace lz {
struct drop_adaptor {
#ifdef LZ_HAS_CXX_11
    static drop_adaptor drop;
#endif

    using adaptor = drop_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 detail::drop_iterable<Iterable>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> n) const {
        return { std::forward<Iterable>(iterable), n };
    }

    template<class DiffT>
    LZ_NODISCARD constexpr detail::fn_args_holder<drop_adaptor, DiffT> operator()(const DiffT n) const {
        return { n };
    }
};

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to drop the first n elements of an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return the same iterator as its input iterable. If its input iterable has a .size()
 * method, then this iterable will also have a .size() method. Example:
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::drop(vec, 2); // res = {3, 4, 5}
 * // or
 * auto res = vec | lz::drop(2); // res = {3, 4, 5}
 * ```
 */
drop_adaptor drop_adaptor::drop{};

#else

/**
 * @brief This adaptor is used to drop the first n elements of an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return the same iterator as its input iterable. If its input iterable has a .size()
 * method, then this iterable will also have a .size() method.Example:
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::drop(vec, 2); // res = {3, 4, 5}
 * // or
 * auto res = vec | lz::drop(2); // res = {3, 4, 5}
 * ```
 */
LZ_INLINE_VAR constexpr drop_adaptor drop{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_DROP_HPP