#pragma once

#ifndef LZ_JOIN_ADAPTOR_HPP
#define LZ_JOIN_ADAPTOR_HPP

#include <Lz/detail/iterables/join.hpp>
#include <sstream>

#ifdef LZ_HAS_CHARCONV
#include <charconv>
#endif

namespace lz {
namespace detail {
class join_adaptor {
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 join_iterable<Iterable> operator()(Iterable&& iterable, std::string separator) const {
        return { std::forward<Iterable>(iterable), std::move(separator) };
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_JOIN_ADAPTOR_HPP