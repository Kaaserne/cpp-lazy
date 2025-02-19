#pragma once

#ifndef LZ_INTERSECTION_ITERABLE_HPP
#define LZ_INTERSECTION_ITERABLE_HPP

#include <Lz/detail/iterators/intersection.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/func_container.hpp>
// TODO
namespace lz {
namespace detail {
template<class Iterable, class Iterable2, class Compare>
class intersection_iterable {
    ref_or_view<Iterable> _iterable;
    ref_or_view<Iterable2> _iterable2;
    func_container<Compare> _compare;

public:
    using iterator =
        intersection_iterator<iter_t<Iterable>, sentinel_t<Iterable>, iter_t<Iterable2>, sentinel_t<Iterable2>, Compare>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr intersection_iterable() = default;

    template<class I, class I2>
    constexpr intersection_iterable(I&& iterable, I2&& iterable2, Compare compare) :
        _iterable{ std::forward<I>(iterable) },
        _iterable2{ std::forward<I2>(iterable2) },
        _compare{ std::move(compare) } {
    }


};
}
} // namespace lz

#endif // LZ_INTERSECTION_ITERABLE_HPP