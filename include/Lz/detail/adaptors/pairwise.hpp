#pragma once

#ifndef LZ_PAIRWISE_ADAPTOR_HPP
#define LZ_PAIRWISE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/pairwise.hpp>

namespace lz {
namespace detail {

struct pairwise_adaptor {
    using adaptor = pairwise_adaptor;
    /**
     * @brief Creates a pairwise iterable with pairs of size @p pair_size from the given @p iterable
     * ```cpp
     * auto vec = std::vector{1, 2, 3, 4, 5};
     * auto paired = lz::pairwise(vec, 2); // {{1, 2}, {2, 3}, {3, 4}, {4, 5}}
     * ```
     * @param iterable The iterable to create the pairwise iterable from
     * @param pair_size The size of the pairs
     * @return A pairwise iterable with pairs of size @p pair_size
     */
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 pairwise_iterable<remove_ref_t<Iterable>>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> pair_size) const {
        return { std::forward<Iterable>(iterable), pair_size };
    }

    /**
     * @brief Creates a function object that can be used to create a pairwise iterable with pairs of size @p pair_size
     * from the given iterable
     * ```cpp
     * auto vec = std::vector{1, 2, 3, 4, 5};
     * auto pairwise_by_2 = vec | lz::pairwise(2); // {{1, 2}, {2, 3}, {3, 4}, {4, 5}}
     * ```
     * @param pair_size The size of the pairs
     * @return A function object that can be used to create a pairwise iterable with pairs of size @p pair_size
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, ptrdiff_t> operator()(const ptrdiff_t pair_size) const {
        return { pair_size };
    }
};

} // namespace detail
} // namespace lz

#endif
