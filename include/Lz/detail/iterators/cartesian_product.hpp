#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <numeric>

namespace lz {
namespace detail {

template<class IterTuple, class SentinelTuple>
class cartesian_product_iterator;

template<class IterTuple, class SentinelTuple>
using default_sentinel_selector = sentinel_selector<iter_tuple_iter_cat_t<IterTuple>,
                                                    cartesian_product_iterator<IterTuple, SentinelTuple>>;

template<class IterTuple, class SentinelTuple>
class cartesian_product_iterator
    : public iterator<cartesian_product_iterator<IterTuple, SentinelTuple>, iter_tuple_ref_type_t<IterTuple>,
                      fake_ptr_proxy<iter_tuple_ref_type_t<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                      iter_tuple_iter_cat_t<IterTuple>, default_sentinel_selector<IterTuple, SentinelTuple>> {

    static constexpr std::size_t tup_size = std::tuple_size<IterTuple>::value;

public:
    using value_type = iter_tuple_value_type_t<IterTuple>;
    using reference = iter_tuple_ref_type_t<IterTuple>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;

private:
    IterTuple _begin;
    IterTuple _iterator;
    SentinelTuple _end;

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> next() {
        auto& first = std::get<0>(_iterator);
        if (first == std::get<0>(_end)) {
            return;
        }
        ++first;
    }

    template<std::size_t I>
    constexpr enable_if<I == 0> previous() const noexcept {
    }

#ifdef LZ_MSVC
#pragma warning(push)
#pragma warning(disable : 4127)
#endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0)> next() {
        auto& prev = std::get<I>(_iterator);
        ++prev;
        if (prev == std::get<I>(_end)) {
            next<I - 1>();

            if (std::get<0>(_iterator) == std::get<0>(_end)) {
                return;
            }

            prev = std::get<I>(_begin);
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 void do_prev() {
        --std::get<I>(_iterator);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> do_prev_all() {
        do_prev<0>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0)> do_prev_all() {
        do_prev<I>();
        do_prev_all<I - 1>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_17 enable_if<(I > 0)> previous() {
        if (_iterator == _end) {
            do_prev_all<I - 1>();
        }
        else {
            auto& prev = std::get<I - 1>(_iterator);
            if (prev == std::get<I - 1>(_begin)) {
                if (I != 1) {
                    prev = std::get<I - 1>(_end);
                    do_prev<I - 1>();
                    previous<I - 1>();
                }
            }
            else {
                do_prev<I - 1>();
            }
        }
    }

#ifdef LZ_MSVC
#pragma warning(pop)
#endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_17 enable_if<I == 0> operator_plus_impl(difference_type offset) {
        auto& it = std::get<I>(_iterator);
        const auto& begin = std::get<I>(_begin);
        auto end = std::get<I>(_end);
        const auto distance = std::distance(begin, end);

        if (_iterator == _end && offset < 0) {
            do_prev_all<I>();
            ++offset;
        }

        const auto to_add = offset % distance;
        if (offset == distance) {
            it = std::move(end);
            return;
        }
        it += to_add;
    }

    template<std::size_t I>
    enable_if<(I > 0)> operator_plus_impl(difference_type offset) {
        auto& it = std::get<I>(_iterator);
        const auto& begin = std::get<I>(_begin);
        auto end = std::get<I>(_end);
        const auto distance = std::distance(begin, end);

        if (_iterator == _end && offset < 0) {
            do_prev_all<I>();
            ++offset;
        }

        const auto to_add = offset % distance;
        it += to_add;
        operator_plus_impl<I - 1>(offset / distance);
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<Is...>) const {
        return reference{ *std::get<Is>(_iterator)... };
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 difference_type distance_impl(index_sequence<Is...>, const cartesian_product_iterator& c) const {
        const difference_type distances[] = { static_cast<difference_type>(std::get<Is>(_iterator) -
                                                                           std::get<Is>(c._iterator))... };
        const difference_type sizes[] = { std::distance(std::get<Is>(_begin), std::get<Is>(_end))... };

        difference_type sum = distances[0];
        for (std::size_t i = 1; i < tup_size; i++) {
            const auto same_sizes = sizes[i] == (distances[i] < 0 ? -distances[i] : distances[i]);
            sum = sum * sizes[i] + (same_sizes ? 0 : distances[i]);
        }
        return sum;
    }

    using index_sequence_for_this = make_index_sequence<tup_size>;

public:
    constexpr cartesian_product_iterator() = default;

    LZ_CONSTEXPR_CXX_14
    cartesian_product_iterator(IterTuple it, IterTuple begin, SentinelTuple end) :
        _begin{ std::move(begin) },
        _iterator{ std::move(it) },
        _end{ std::move(end) } {
        static_assert(tup_size > 1, "Cannot cartesian product one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference(index_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        next<tup_size - 1>();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        previous<tup_size>();
    }

    void plus_is(const difference_type n) {
        operator_plus_impl<tup_size - 1>(n);
        if (std::get<0>(_iterator) == std::get<0>(_end)) {
            _iterator = _end;
        }
    }

    constexpr bool eq(const cartesian_product_iterator& other) const {
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const cartesian_product_iterator& other) const {
        return distance_impl(index_sequence_for_this{}, other);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
