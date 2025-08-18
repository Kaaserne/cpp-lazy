#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {

template<class... Iterables>
struct iter_maybe_homogeneous;

template<class... Iterables>
struct iter_maybe_homogeneous<std::tuple<Iterables...>> {
    using type = std::tuple<iter_t<Iterables>...>;
};

template<class Iterable, size_t N>
struct iter_maybe_homogeneous<homogeneous_array<Iterable, N>> {
    using type = homogeneous_array<iter_t<Iterable>, N>;
};

template<class Iterables>
using iter_t_maybe_homogeneous_t = typename iter_maybe_homogeneous<Iterables>::type;

template<class IterablesMaybeHomo>
class cartesian_product_iterator
    : public iterator<cartesian_product_iterator<IterablesMaybeHomo>,
                      iter_tuple_ref_type_t<iter_t_maybe_homogeneous_t<IterablesMaybeHomo>>,
                      fake_ptr_proxy<iter_tuple_ref_type_t<iter_t_maybe_homogeneous_t<IterablesMaybeHomo>>>,
                      iter_tuple_diff_type_t<iter_t_maybe_homogeneous_t<IterablesMaybeHomo>>,
                      iter_tuple_iter_cat_t<iter_t_maybe_homogeneous_t<IterablesMaybeHomo>>, default_sentinel_t> {

    using iterators = iter_t_maybe_homogeneous_t<IterablesMaybeHomo>;
    static constexpr size_t tup_size = tuple_size<iterators>::value;

public:
    using value_type = iter_tuple_value_type_t<iterators>;
    using reference = iter_tuple_ref_type_t<iterators>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = iter_tuple_diff_type_t<iterators>;

private:
    iterators _iterators;
    IterablesMaybeHomo _iterables;

#ifdef LZ_HAS_CXX_17

    template<size_t I>
    constexpr void next() {
        using std::get;
        ++get<I>(_iterators);
        if constexpr (I > 0) {
            if (get<I>(_iterators) == get<I>(_iterables).end()) {
                get<I>(_iterators) = get<I>(_iterables).begin();
                next<I - 1>();
            }
        }
    }

    template<size_t I>
    constexpr void previous() {
        using std::get;
        if constexpr (I > 0) {
            if (get<I>(_iterators) == get<I>(_iterables).begin()) {
                get<I>(_iterators) = get<I>(_iterables).end();
                previous<I - 1>();
            }
        }
        LZ_ASSERT_DECREMENTABLE(get<I>(_iterators) != get<I>(_iterables).begin());
        --get<I>(_iterators);
    }

    template<size_t I>
    constexpr void operator_plus_impl(const difference_type offset) {
        using std::get;
        if constexpr (I == 0) {
            LZ_ASSERT_SUB_ADDABLE(offset < 0 ? -offset <= get<0>(_iterators) - get<0>(_iterables).begin()
                                             : offset <= get<0>(_iterables).end() - get<0>(_iterators));
            get<0>(_iterators) += offset;
        }
        else {
            if (offset == 0) {
                return;
            }

            const auto size = static_cast<difference_type>(get<I>(_iterables).end() - get<I>(_iterables).begin());
            const auto iterator_offset = static_cast<difference_type>(get<I>(_iterators) - get<I>(_iterables).begin());
            const auto to_add_this = (iterator_offset + offset) % size;
            const auto to_add_next = (iterator_offset + offset) / size;

            if (to_add_this < 0) {
                get<I>(_iterators) = get<I>(_iterables).begin() + static_cast<difference_type>(to_add_this + size);
                operator_plus_impl<I - 1>(to_add_next - 1);
                return;
            }

            get<I>(_iterators) = get<I>(_iterables).begin() + static_cast<difference_type>(to_add_this);
            operator_plus_impl<I - 1>(to_add_next);
        }
    }

#else

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0> next() {
        using std::get;
        ++get<I>(_iterators);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I > 0)> next() {
        using std::get;
        ++get<I>(_iterators);
        if (get<I>(_iterators) == get<I>(_iterables).end()) {
            get<I>(_iterators) = get<I>(_iterables).begin();
            next<I - 1>();
        }
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0> previous() {
        using std::get;
        LZ_ASSERT_DECREMENTABLE(get<0>(_iterators) != get<0>(_iterables).begin());
        --get<0>(_iterators);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I > 0)> previous() {
        using std::get;
        if (get<I>(_iterators) == get<I>(_iterables).begin()) {
            get<I>(_iterators) = get<I>(_iterables).end();
            previous<I - 1>();
        }
        LZ_ASSERT_DECREMENTABLE(get<I>(_iterators) != get<I>(_iterables).begin());
        --get<I>(_iterators);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0> operator_plus_impl(const difference_type offset) {
        using std::get;
        LZ_ASSERT_SUB_ADDABLE(offset < 0 ? -offset <= get<0>(_iterators) - get<0>(_iterables).begin()
                                         : offset <= get<0>(_iterables).end() - get<0>(_iterators));
        get<0>(_iterators) += offset;
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I > 0)> operator_plus_impl(const difference_type offset) {
        using std::get;
        if (offset == 0) {
            return;
        }

        const auto size = static_cast<difference_type>(get<I>(_iterables).end() - get<I>(_iterables).begin());
        const auto iterator_offset = static_cast<difference_type>(get<I>(_iterators) - get<I>(_iterables).begin());
        const auto to_add_this = (iterator_offset + offset) % size;
        const auto to_add_next = (iterator_offset + offset) / size;

        if (to_add_this < 0) {
            get<I>(_iterators) = get<I>(_iterables).begin() + static_cast<difference_type>(to_add_this + size);
            operator_plus_impl<I - 1>(to_add_next - 1);
            return;
        }

        get<I>(_iterators) = get<I>(_iterables).begin() + static_cast<difference_type>(to_add_this);
        operator_plus_impl<I - 1>(to_add_next);
    }

#endif // LZ_HAS_CXX_17

    template<size_t... Is>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<Is...>) const {
        using std::get;
        return reference{ *get<Is>(_iterators)... };
    }

#ifdef LZ_HAS_CXX_17

    template<std::ptrdiff_t I>
    constexpr void iter_compat(const cartesian_product_iterator& other) const {
        using std::get;
        if constexpr (I >= 0) {
            LZ_ASSERT_COMPATIBLE(get<I>(_iterables).begin() == get<I>(other._iterables).begin() &&
                                 get<I>(_iterables).end() == get<I>(other._iterables).end());
            iter_compat<I - 1>(other);
        }
    }

    template<size_t I>
    constexpr difference_type difference(const cartesian_product_iterator& other) const {
        using std::get;
        if constexpr (I > 0) {
            const auto distance = get<I>(_iterators) - get<I>(other._iterators);
            const auto size = get<I>(_iterables).end() - get<I>(_iterables).begin();
            return size * difference<I - 1>(other) + distance;
        }
        else {
            return get<0>(_iterators) - get<0>(other._iterators);
        }
    }

    template<size_t I>
    constexpr difference_type difference() const {
        using std::get;
        if constexpr (I > 0) {
            const auto distance = get<I>(_iterators) - get<I>(_iterables).begin();
            const auto size = get<I>(_iterables).end() - get<I>(_iterables).begin();
            return size * difference<I - 1>() + distance;
        }
        else {
            return get<I>(_iterators) - get<I>(_iterables).end();
        }
    }

#else

    template<std::ptrdiff_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I >= 0)> iter_compat(const cartesian_product_iterator& other) const {
        using std::get;
        LZ_ASSERT_COMPATIBLE(get<I>(_iterables).begin() == get<I>(other._iterables).begin() &&
                             get<I>(_iterables).end() == get<I>(other._iterables).end());
        iter_compat<I - 1>(other);
    }

    template<std::ptrdiff_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I < 0)> iter_compat(const cartesian_product_iterator&) const noexcept {
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I > 0), difference_type> difference(const cartesian_product_iterator& other) const {
        using std::get;
        const auto distance = get<I>(_iterators) - get<I>(other._iterators);
        const auto size = get<I>(_iterables).end() - get<I>(_iterables).begin();
        const auto result = size * difference<I - 1>(other) + distance;
        return result;
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0, difference_type> difference(const cartesian_product_iterator& other) const {
        using std::get;
        return get<0>(_iterators) - get<0>(other._iterators);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I > 0), difference_type> difference() const {
        using std::get;
        const auto distance = get<I>(_iterators) - get<I>(_iterables).begin();
        const auto size = get<I>(_iterables).end() - get<I>(_iterables).begin();
        return size * difference<I - 1>() + distance;
    }

    template<size_t I>
    constexpr enable_if_t<I == 0, difference_type> difference() const {
        using std::get;
        return get<I>(_iterators) - get<I>(_iterables).end();
    }

#endif

    using is = make_index_sequence<tup_size>;

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) = get<I>(_iterables).end()), ...);
#else
        decompose(get<I>(_iterators) = get<I>(_iterables).end()...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr cartesian_product_iterator()
        requires(std::default_initializable<iterators> && std::default_initializable<IterablesMaybeHomo>)
    = default;

#else

    template<class I = iterators, class = enable_if_t<std::is_default_constructible<I>::value &&
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
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return dereference(is{});
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        next<tup_size - 1>();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        previous<tup_size - 1>();
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
        operator_plus_impl<tup_size - 1>(n);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const cartesian_product_iterator& other) const {
        iter_compat<static_cast<std::ptrdiff_t>(tup_size) - 1>(other);
        return _iterators == other._iterators;
    }

    constexpr bool eq(default_sentinel_t) const {
        using std::get;
        return get<0>(_iterators) == get<0>(_iterables).end();
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const cartesian_product_iterator& other) const {
        iter_compat<static_cast<std::ptrdiff_t>(tup_size) - 1>(other);
        return difference<tup_size - 1>(other);
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return difference<tup_size - 1>();
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
