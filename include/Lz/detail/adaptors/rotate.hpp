#pragma once

#ifndef LZ_ROTATE_ADAPTOR_HPP
#define LZ_ROTATE_ADAPTOR_HPP

#include <Lz/detail/iterables/rotate.hpp>

namespace lz {
namespace detail {
struct rotate_adaptor {
    using adaptor = rotate_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr rotate_iterable<Iterable>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> start) const {
        return { std::forward<Iterable>(iterable), start };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::ptrdiff_t> operator()(const std::ptrdiff_t start) const {
        return { start };
    }
};
} // namespace detail
} // namespace lz

#endif
