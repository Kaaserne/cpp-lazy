#pragma once

#ifndef LZ_DROP_ADAPTOR_HPP
#define LZ_DROP_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/drop.hpp>

namespace lz {
namespace detail {
struct drop_adaptor {
    using adaptor = drop_adaptor;

    /**
     * @brief This adaptor is used to drop the first n elements of an iterable. The iterator category is the same as the input
     * iterator category. Its end() function will return the same type as its input iterable. If its input iterable has a
     * .size() method, then this iterable will also have a .size() method. If the amount to drop is larger than the size, size
     * will be 0. Example:
     * ```cpp
     * auto vec = std::vector<int>{1, 2, 3, 4, 5};
     * auto res = lz::drop(vec, 2); // res = {3, 4, 5}
     * auto res = lz::drop(vec, 50); // res = {}, size = 0
     * ```
     * @param iterable The iterable to drop elements from
     * @param n The amount of elements to drop
     * @return A drop_iterable that will drop the first n elements of the input iterable
     */
    template<class Iterable>
    LZ_NODISCARD constexpr drop_iterable<remove_ref_t<Iterable>>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> n) const {
        return { iterable, n };
    }

    /**
     * @brief This adaptor is used to drop the first n elements of an iterable. The iterator category is the same as the input
     * iterator category. Its end() function will return the same type as its input iterable. If its input iterable has a
     * .size() method, then this iterable will also have a .size() method. If the amount to drop is larger than the size, size
     * will be 0. Example:
     * ```cpp
     * auto vec = std::vector<int>{1, 2, 3, 4, 5};
     * auto res = vec | lz::drop(2); // res = {3, 4, 5}
     * auto res = vec | lz::drop(50); // res = {}, size = 0
     * ```
     * @param n The amount of elements to drop
     * @return An adaptor that can be used with pipe expressions
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, ptrdiff_t> operator()(const ptrdiff_t n) const {
        return { n };
    }
};
} // namespace lz
} // namespace detail

#endif
