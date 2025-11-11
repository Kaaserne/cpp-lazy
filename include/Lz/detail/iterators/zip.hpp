#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/sentinel_with.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/procs/decompose.hpp>
#endif

namespace lz {
namespace detail {
template<class... Iterables>
class zip_iterator : public iterator<zip_iterator<Iterables...>, iter_tuple_ref_type_t<maybe_homogeneous_t<iter_t<Iterables>...>>,
                                     fake_ptr_proxy<iter_tuple_ref_type_t<maybe_homogeneous_t<iter_t<Iterables>...>>>,
                                     iter_tuple_diff_type_t<maybe_homogeneous_t<iter_t<Iterables>...>>,
                                     iter_tuple_iter_cat_t<maybe_homogeneous_t<iter_t<Iterables>...>>,
                                     sentinel_with<maybe_homogeneous_t<sentinel_t<Iterables>...>>> {

    using iter_tuple = maybe_homogeneous_t<iter_t<Iterables>...>;

public:
    using iterator_category = iter_tuple_iter_cat_t<iter_tuple>;
    using value_type = iter_tuple_value_type_t<iter_tuple>;
    using difference_type = iter_tuple_diff_type_t<iter_tuple>;
    using reference = iter_tuple_ref_type_t<iter_tuple>;
    using pointer = fake_ptr_proxy<reference>;
    using sentinel = sentinel_with<maybe_homogeneous_t<sentinel_t<Iterables>...>>;

private:
    using is = make_index_sequence<tuple_size<iter_tuple>::value>;
    iter_tuple _iterators{};

    template<size_t... I>
    constexpr reference dereference(index_sequence<I...>) const {
        using std::get;
        return reference{ *get<I>(_iterators)... };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        (++get<I>(_iterators), ...);
#else
        decompose(++get<I>(_iterators)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        (--get<I>(_iterators), ...);
#else
        decompose(--get<I>(_iterators)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset, index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) += offset), ...);
#else
        decompose(get<I>(_iterators) += offset...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const zip_iterator& other, index_sequence<I...>) const {
        using std::get;
        const auto max = std::max({ (get<I>(_iterators) - get<I>(other._iterators))... });
        if (max > 0) {
            return max;
        }
        return std::min({ (get<I>(_iterators) - get<I>(other._iterators))... });
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const sentinel& other, index_sequence<I...>) const {
        using std::get;
        return std::max({ (get<I>(_iterators) - get<I>(other.value))... });
    }

    template<class EndIter, size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(const EndIter& other, index_sequence<I...>) const {
        using std::get;
#ifdef LZ_HAS_CXX_17
        return ((get<I>(_iterators) == get<I>(other)) || ...);
#else
        return std::max({ (get<I>(_iterators) == get<I>(other))... });
#endif
    }

    template<class cat = iterator_category, size_t... I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi_tag<cat>::value> assign_sentinels(const sentinel& other, index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) = get<I>(other.value)), ...);
#else
        decompose(get<I>(_iterators) = get<I>(other.value)...);
#endif
    }

    template<class cat = iterator_category, size_t... I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<is_bidi_tag<cat>::value && !is_ra_tag<cat>::value>
    assign_sentinels(const sentinel& other, index_sequence<I...>) {
        while (*this != other) {
            ++(*this);
        }
    }

    template<class cat = iterator_category, size_t... I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra_tag<cat>::value> assign_sentinels(const sentinel& other, index_sequence<I...>) {
        *this += (other - *this);
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_iterator()
        requires(std::default_initializable<iter_tuple>)
    = default;

#else

    template<class I = iter_tuple, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr zip_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr zip_iterator(iter_tuple iterators) : _iterators{ std::move(iterators) } {
        static_assert(tuple_size<iter_tuple>::value > 1, "Cannot concat one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 zip_iterator& operator=(const sentinel& end) {
        assign_sentinels(end, is{});
        return *this;
    }

    constexpr reference dereference() const {
        return dereference(is{});
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(is{});
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        decrement(is{});
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        plus_is(offset, is{});
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_iterator& other) const {
        return minus(other, is{});
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const sentinel& other) const {
        return minus(other, is{});
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const zip_iterator& other) const {
        return eq(other._iterators, is{});
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const sentinel& other) const {
        return eq(other.value, is{});
    }
};
} // namespace detail
} // namespace lz

#endif
