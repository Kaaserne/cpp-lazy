#pragma once

#ifndef LZ_PAIRWISE_ADAPTOR_HPP
#define LZ_PAIRWISE_ADAPTOR_HPP


#include <Lz/detail/iterables/pairwise.hpp>

namespace lz {
namespace detail {

struct pairwise_adaptor {
    using adaptor = pairwise_adaptor;

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 pairwise_iterable<remove_ref_t<Iterable>>
    operator()(Iterable&& iterable, const size_t pair_size) const {
        return { std::forward<Iterable>(iterable), pair_size };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, size_t> operator()(const size_t pair_size) const {
        return { pair_size };
    }
};

} // namespace detail
} // namespace lz

#endif
