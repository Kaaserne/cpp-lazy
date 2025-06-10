#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class, class>
class cartesian_product_iterator;

template<class IterTuple, class STuple>
using default_sentinel_selector =
    sentinel_selector<iter_tuple_iter_cat_t<IterTuple>, cartesian_product_iterator<IterTuple, STuple>>;

template<class IterTuple, class STuple>
class cartesian_product_iterator
    : public iterator<cartesian_product_iterator<IterTuple, STuple>, iter_tuple_ref_type_t<IterTuple>,
                      fake_ptr_proxy<iter_tuple_ref_type_t<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                      iter_tuple_iter_cat_t<IterTuple>, default_sentinel_selector<IterTuple, STuple>> {

    static constexpr std::size_t tup_size = std::tuple_size<IterTuple>::value;

public:
    using value_type = iter_tuple_value_type_t<IterTuple>;
    using reference = iter_tuple_ref_type_t<IterTuple>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;

private:
    IterTuple _begin;
    IterTuple _iterators;
    STuple _end;

#ifdef LZ_HAS_CXX_17

    template<std::size_t I>
    constexpr void next() {
        ++std::get<I>(_iterators);
        if constexpr (I > 0) {
            if (std::get<I>(_iterators) == std::get<I>(_end)) {
                std::get<I>(_iterators) = std::get<I>(_begin);
                next<I - 1>();
            }
        }
    }

    template<std::size_t I>
    constexpr void previous() {
        if constexpr (I > 0) {
            if (std::get<I>(_iterators) == std::get<I>(_begin)) {
                std::get<I>(_iterators) = std::get<I>(_end);
                previous<I - 1>();
            }
        }
        --std::get<I>(_iterators);
    }

    template<std::size_t I>
    constexpr void operator_plus_impl(const difference_type offset) {
        if constexpr (I == 0) {
            std::get<0>(_iterators) += offset;
        }
        else {
            if (offset == 0) {
                return;
            }

            const auto size = static_cast<difference_type>(std::get<I>(_end) - std::get<I>(_begin));
            const auto iterator_offset = static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin));
            const auto to_add_this = (iterator_offset + offset) % size;
            const auto to_add_next = (iterator_offset + offset) / size;

            if (to_add_this < 0) {
                std::get<I>(_iterators) = std::get<I>(_begin) + static_cast<difference_type>(to_add_this + size);
                operator_plus_impl<I - 1>(to_add_next - 1);
                return;
            }

            std::get<I>(_iterators) = std::get<I>(_begin) + static_cast<difference_type>(to_add_this);
            operator_plus_impl<I - 1>(to_add_next);
        }
    }

#else

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> next() {
        ++std::get<I>(_iterators);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0)> next() {
        ++std::get<I>(_iterators);
        if (std::get<I>(_iterators) == std::get<I>(_end)) {
            std::get<I>(_iterators) = std::get<I>(_begin);
            next<I - 1>();
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> previous() {
        --std::get<I>(_iterators);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0)> previous() {
        if (std::get<I>(_iterators) == std::get<I>(_begin)) {
            std::get<I>(_iterators) = std::get<I>(_end);
            previous<I - 1>();
        }
        --std::get<I>(_iterators);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> operator_plus_impl(const difference_type offset) {
        std::get<0>(_iterators) += offset;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0)> operator_plus_impl(const difference_type offset) {
        if (offset == 0) {
            return;
        }

        const auto size = static_cast<difference_type>(std::get<I>(_end) - std::get<I>(_begin));
        const auto iterator_offset = static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin));
        const auto to_add_this = (iterator_offset + offset) % size;
        const auto to_add_next = (iterator_offset + offset) / size;

        if (to_add_this < 0) {
            std::get<I>(_iterators) = std::get<I>(_begin) + static_cast<difference_type>(to_add_this + size);
            operator_plus_impl<I - 1>(to_add_next - 1);
            return;
        }

        std::get<I>(_iterators) = std::get<I>(_begin) + static_cast<difference_type>(to_add_this);
        operator_plus_impl<I - 1>(to_add_next);
    }

#endif // LZ_HAS_CXX_17

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<Is...>) const {
        return reference{ *std::get<Is>(_iterators)... };
    }

#ifdef LZ_HAS_CXX_17

    template<std::size_t I>
    constexpr difference_type distance_impl(const cartesian_product_iterator& other) const {
        if constexpr (I > 0) {
            const auto distance = std::get<I>(_iterators) - std::get<I>(other._iterators);
            const auto size = std::get<I>(_end) - std::get<I>(_begin);
            const auto result = size * distance_impl<I - 1>(other) + distance;
            return result;
        }
        else {
            return std::get<0>(_iterators) - std::get<0>(other._iterators);
        }
    }

#else

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0), difference_type> distance_impl(const cartesian_product_iterator& other) const {
        const auto distance = std::get<I>(_iterators) - std::get<I>(other._iterators);
        const auto size = std::get<I>(_end) - std::get<I>(_begin);
        const auto result = size * distance_impl<I - 1>(other) + distance;
        return result;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0, difference_type> distance_impl(const cartesian_product_iterator& other) const {
        return std::get<0>(_iterators) - std::get<0>(other._iterators);
    }

#endif

    using index_sequence_for_this = make_index_sequence<tup_size>;

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
        decompose(std::get<I>(_iterators) = std::get<I>(_end)...);
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr cartesian_product_iterator()
        requires std::default_initializable<IterTuple> && std::default_initializable<STuple>
    = default;

#else

    template<class I = IterTuple,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<STuple>::value>>
    constexpr cartesian_product_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                                    std::is_nothrow_default_constructible<Iterables>::value) {
    }

#endif
    LZ_CONSTEXPR_CXX_14
    cartesian_product_iterator(IterTuple iterators, IterTuple begin, STuple end) :
        _begin{ std::move(begin) },
        _iterators{ std::move(iterators) },
        _end{ std::move(end) } {
        static_assert(tup_size > 1, "Cannot cartesian product one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 cartesian_product_iterator& operator=(default_sentinel) {
        assign_sentinels(index_sequence_for_this{});
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference(index_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        next<tup_size - 1>();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        previous<tup_size - 1>();
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
        operator_plus_impl<tup_size - 1>(n);
    }

    constexpr bool eq(const cartesian_product_iterator& other) const {
        return _iterators == other._iterators;
    }

    constexpr bool eq(default_sentinel) const {
        return std::get<0>(_iterators) == std::get<0>(_end);
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const cartesian_product_iterator& other) const {
        return distance_impl<tup_size - 1>(other);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
