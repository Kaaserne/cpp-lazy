#pragma once

#ifndef LZ_JOIN_ITERABLE_HPP
#define LZ_JOIN_ITERABLE_HPP

#include <Lz/detail/iterators/join.hpp>

namespace lz {
namespace detail {

template<class Iterable, class UnaryFormatFn>
class join_iterable : public lazy_view {
    std::string _separator;
    ref_or_view<Iterable> _iterable;

public:
    using iterator = join_iterator<iter_t<Iterable>, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr join_iterable() = default;

    template<class I>
    LZ_CONSTEXPR_CXX_20 join_iterable(I&& iterable, std::string separator) :
        _separator{ std::move(separator) },
        _iterable{ iterable } {
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_JOIN_ITERABLE_HPP