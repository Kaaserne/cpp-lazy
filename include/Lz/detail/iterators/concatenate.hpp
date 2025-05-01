#pragma once

#ifndef LZ_CONCATENATE_ITERATOR_HPP
#define LZ_CONCATENATE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <numeric>

namespace lz {
namespace detail {

template<class Tuple>
using first_it = tup_element<0, Tuple>;

template<class IterTuple, class SentinelTuple>
class concatenate_iterator
    : public iterator<concatenate_iterator<IterTuple, SentinelTuple>, ref_t<first_it<IterTuple>>,
                      fake_ptr_proxy<ref_t<first_it<IterTuple>>>, iter_tuple_diff_type_t<IterTuple>,
                      iter_tuple_iter_cat_t<IterTuple>,
                      sentinel_selector<iter_tuple_iter_cat_t<IterTuple>, concatenate_iterator<IterTuple, SentinelTuple>>> {

    IterTuple _iterators;
    IterTuple _begin;
    SentinelTuple _end;

    using first_tuple_iterator = std::iterator_traits<first_it<IterTuple>>;

    static constexpr std::size_t tuple_size = std::tuple_size<IterTuple>::value;

public:
    using value_type = typename first_tuple_iterator::value_type;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = typename first_tuple_iterator::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(index_sequence<I...>, const concatenate_iterator& other) const {
        const difference_type totals[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                        std::get<I>(other._iterators))... };
        return std::accumulate(std::begin(totals), std::end(totals), difference_type{ 0 });
    }

    template<std::size_t I, class DifferenceType>
    LZ_CONSTEXPR_CXX_14 enable_if<I != 0> min_is_n(const DifferenceType offset) {
        auto& current = std::get<I>(_iterators);
        auto& begin = std::get<I>(_begin);

        if (current != begin) {
            const auto dist = current - begin;
            if (dist <= offset) {
                current = begin;
                min_is_n<I - 1>(dist == 0 ? 1 : offset - dist);
            }
            else {
                current -= offset;
            }
        }
        else {
            min_is_n<I - 1>(offset);
        }
    }

    template<std::size_t I, class DifferenceType>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> min_is_n(const DifferenceType offset) {
        auto& current = std::get<0>(_iterators);
        current -= offset;
    }

    template<std::size_t I, class DifferenceType>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tuple_size> plus_is_n(const DifferenceType offset) {
        auto& current = std::get<I>(_iterators);
        const auto& current_end = std::get<I>(_end);
        const auto dist = current_end - current;

        if (dist > offset) {
            current += offset;
        }
        else {
            // Moves to end
            current += dist;
            plus_is_n<I + 1>(offset - dist);
        }
    }

    template<std::size_t I, class DifferenceType>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size> plus_is_n(const DifferenceType) const noexcept {
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != 0> minus_minus() {
        auto& current = std::get<I>(_iterators);
        if (current != std::get<I>(_begin)) {
            --current;
        }
        else {
            minus_minus<I - 1>();
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> minus_minus() {
        --std::get<0>(_iterators);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 auto deref() const -> enable_if<I == tuple_size - 1, decltype(*std::get<I>(_iterators))> {
        return *std::get<I>(_iterators);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 auto deref() const -> enable_if<I != tuple_size - 1, decltype(*std::get<I>(_iterators))> {
        if (std::get<I>(_iterators) != std::get<I>(_end)) {
            return *std::get<I>(_iterators);
        }
        else {
            return deref<I + 1>();
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != std::tuple_size<IterTuple>::value> plus_plus() {
        if (std::get<I>(_iterators) != std::get<I>(_end)) {
            ++std::get<I>(_iterators);
        }
        else {
            plus_plus<I + 1>();
        }
    }

    template<std::size_t I>
    constexpr enable_if<I == std::tuple_size<IterTuple>::value> plus_plus() const noexcept {
        return;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != std::tuple_size<IterTuple>::value - 1, bool>
    iter_equal_to(const SentinelTuple& end) const {
        const auto has_value = std::get<I>(_iterators) == std::get<I>(end);
        return has_value ? iter_equal_to<I + 1>(end) : has_value;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == std::tuple_size<IterTuple>::value - 1, bool>
    iter_equal_to(const SentinelTuple& end) const {
        return std::get<I>(_iterators) == std::get<I>(end);
    }

public:
    LZ_CONSTEXPR_CXX_14 concatenate_iterator(IterTuple iterators, IterTuple begin, SentinelTuple end) :
        _iterators{ std::move(iterators) },
        _begin{ std::move(begin) },
        _end{ std::move(end) } {
        static_assert(tuple_size > 1, "Cannot concat one/zero iterables");
    }

    constexpr concatenate_iterator() = default;

    LZ_CONSTEXPR_CXX_14 concatenate_iterator& operator=(default_sentinel) {
        _iterators = _end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return deref<0>();
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        plus_plus<0>();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        minus_minus<tuple_size - 1>();
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        if (offset < 0) {
            min_is_n<tuple_size - 1>(-offset);
        }
        else {
            plus_is_n<0>(offset);
        }
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const concatenate_iterator& other) const {
        LZ_ASSERT(_end == other._end && _begin == other._begin, "Incompatible iterators");
        return minus(make_index_sequence<tuple_size>(), other);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const concatenate_iterator& b) const {
        // static_assert(tuple_size > 1, "Cannot concat one iterable");
        LZ_ASSERT(_end == b._end && _begin == b._begin, "Incompatible iterators");
        return iter_equal_to<0>(b._iterators);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return iter_equal_to<0>(_end);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_CONCATENATE_ITERATOR_HPP