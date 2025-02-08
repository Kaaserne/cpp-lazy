#pragma once

#ifndef LZ_CONCATENATE_ADAPTOR_HPP
#define LZ_CONCATENATE_ADAPTOR_HPP

#include <Lz/detail/iterables/concatenate.hpp>

namespace lz {
namespace detail {
struct concatenate_adaptor {
    template<class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 concatenate_iterable<Iterables...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif
