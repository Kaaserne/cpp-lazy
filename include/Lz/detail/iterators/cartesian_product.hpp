#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class... Iterables>
struct iter_t_maybe_homogeneous_helper;

template<class... Iterables>
struct iter_t_maybe_homogeneous_helper<std::tuple<Iterables...>> {
    using type = std::tuple<iter_t<Iterables>...>;
};

template<class Iterable, std::size_t N>
struct iter_t_maybe_homogeneous_helper<std::array<Iterable, N>> {
    using type = std::array<iter_t<Iterable>, N>;
};

template<class Iterables>
using iter_t_maybe_homogeneous = typename iter_t_maybe_homogeneous_helper<Iterables>::type;

template<class IterablesMaybeHomo>
class cartesian_product_iterator
    : public iterator<cartesian_product_iterator<IterablesMaybeHomo>,
                      iter_tuple_ref_type_t<iter_t_maybe_homogeneous<IterablesMaybeHomo>>,
                      fake_ptr_proxy<iter_tuple_ref_type_t<iter_t_maybe_homogeneous<IterablesMaybeHomo>>>,
                      iter_tuple_diff_type_t<iter_t_maybe_homogeneous<IterablesMaybeHomo>>,
                      iter_tuple_iter_cat_t<iter_t_maybe_homogeneous<IterablesMaybeHomo>>, default_sentinel_t> {

    using iterators = iter_t_maybe_homogeneous<IterablesMaybeHomo>;
    static constexpr std::size_t tup_size = std::tuple_size<iterators>::value;

public:
    using value_type = iter_tuple_value_type_t<iterators>;
    using reference = iter_tuple_ref_type_t<iterators>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = iter_tuple_diff_type_t<iterators>;

private:
    iterators _iterators;
    IterablesMaybeHomo _iterables;

#ifdef LZ_HAS_CXX_17

    template<std::size_t I>
    constexpr void next() {
        ++std::get<I>(_iterators);
        if constexpr (I > 0) {
            if (std::get<I>(_iterators) == std::end(std::get<I>(_iterables))) {
                std::get<I>(_iterators) = std::begin(std::get<I>(_iterables));
                next<I - 1>();
            }
        }
    }

    template<std::size_t I>
    constexpr void previous() {
        if constexpr (I > 0) {
            if (std::get<I>(_iterators) == std::begin(std::get<I>(_iterables))) {
                std::get<I>(_iterators) = std::end(std::get<I>(_iterables));
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

            const auto size =
                static_cast<difference_type>(std::end(std::get<I>(_iterables)) - std::begin(std::get<I>(_iterables)));
            const auto iterator_offset =
                static_cast<difference_type>(std::get<I>(_iterators) - std::begin(std::get<I>(_iterables)));
            const auto to_add_this = (iterator_offset + offset) % size;
            const auto to_add_next = (iterator_offset + offset) / size;

            if (to_add_this < 0) {
                std::get<I>(_iterators) = std::begin(std::get<I>(_iterables)) + static_cast<difference_type>(to_add_this + size);
                operator_plus_impl<I - 1>(to_add_next - 1);
                return;
            }

            std::get<I>(_iterators) = std::begin(std::get<I>(_iterables)) + static_cast<difference_type>(to_add_this);
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
        if (std::get<I>(_iterators) == std::end(std::get<I>(_iterables))) {
            std::get<I>(_iterators) = std::begin(std::get<I>(_iterables));
            next<I - 1>();
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0> previous() {
        --std::get<I>(_iterators);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0)> previous() {
        if (std::get<I>(_iterators) == std::begin(std::get<I>(_iterables))) {
            std::get<I>(_iterators) = std::end(std::get<I>(_iterables));
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

        const auto size = static_cast<difference_type>(std::end(std::get<I>(_iterables)) - std::begin(std::get<I>(_iterables)));
        const auto iterator_offset = static_cast<difference_type>(std::get<I>(_iterators) - std::begin(std::get<I>(_iterables)));
        const auto to_add_this = (iterator_offset + offset) % size;
        const auto to_add_next = (iterator_offset + offset) / size;

        if (to_add_this < 0) {
            std::get<I>(_iterators) = std::begin(std::get<I>(_iterables)) + static_cast<difference_type>(to_add_this + size);
            operator_plus_impl<I - 1>(to_add_next - 1);
            return;
        }

        std::get<I>(_iterators) = std::begin(std::get<I>(_iterables)) + static_cast<difference_type>(to_add_this);
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
            const auto size = std::end(std::get<I>(_iterables)) - std::begin(std::get<I>(_iterables));
            return size * distance_impl<I - 1>(other) + distance;
        }
        else {
            return std::get<0>(_iterators) - std::get<0>(other._iterators);
        }
    }

    template<std::size_t I>
    constexpr difference_type distance_impl() const {
        if constexpr (I > 0) {
            const auto distance = std::get<I>(_iterators) - std::begin(std::get<I>(_iterables));
            const auto size = std::end(std::get<I>(_iterables)) - std::begin(std::get<I>(_iterables));
            return size * distance_impl<I - 1>() + distance;
        }
        else {
            return std::get<I>(_iterators) - std::end(std::get<I>(_iterables));
        }
    }

#else

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I > 0), difference_type> distance_impl(const cartesian_product_iterator& other) const {
        const auto distance = std::get<I>(_iterators) - std::get<I>(other._iterators);
        const auto size = std::end(std::get<I>(_iterables)) - std::begin(std::get<I>(_iterables));
        const auto result = size * distance_impl<I - 1>(other) + distance;
        return result;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0, difference_type> distance_impl(const cartesian_product_iterator& other) const {
        return std::get<0>(_iterators) - std::get<0>(other._iterators);
    }

    template<std::size_t I>
    constexpr enable_if<(I > 0), difference_type> distance_impl() const {
        const auto distance = std::get<I>(_iterators) - std::begin(std::get<I>(_iterables));
        const auto size = std::end(std::get<I>(_iterables)) - std::begin(std::get<I>(_iterables));
        return size * distance_impl<I - 1>() + distance;
    }

    template<std::size_t I>
    constexpr enable_if<I == 0, difference_type> distance_impl() const {
        return std::get<I>(_iterators) - std::end(std::get<I>(_iterables));
    }

#endif

    using is = make_index_sequence<tup_size>;

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) = std::end(std::get<I>(_iterables))), ...);
#else
        decompose(std::get<I>(_iterators) = std::end(std::get<I>(_iterables))...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr cartesian_product_iterator()
        requires std::default_initializable<iterators> && std::default_initializable<IterablesMaybeHomo>
    = default;

#else

    template<class I = iterators, class = enable_if<std::is_default_constructible<I>::value &&
                                                    std::is_default_constructible<IterablesMaybeHomo>::value>>
    constexpr cartesian_product_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                                    std::is_nothrow_default_constructible<IterablesMaybeHomo>::value) {
    }

#endif
    template<class I>
    LZ_CONSTEXPR_CXX_14 cartesian_product_iterator(I&& iterables, iterators iters) :
        _iterators{ std::move(iters) },
        _iterables{ std::forward<I>(iterables) } {
        static_assert(tup_size > 1, "Cannot cartesian product one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 cartesian_product_iterator& operator=(default_sentinel_t) {
        assign_sentinels(is{});
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference(is{});
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
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

    constexpr bool eq(default_sentinel_t) const {
        return std::get<0>(_iterators) == std::end(std::get<0>(_iterables));
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const cartesian_product_iterator& other) const {
        return distance_impl<tup_size - 1>(other);
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return distance_impl<tup_size - 1>();
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
