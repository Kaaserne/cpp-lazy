#pragma once

#ifndef LZ_CONCATENATE_ITERATOR_HPP
#define LZ_CONCATENATE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/util/default_sentinel.hpp>
#include <numeric>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/procs/decompose.hpp>
#endif

namespace lz {
namespace detail {

template<class Iterables, class Iterators>
class concatenate_iterator
    : public iterator<concatenate_iterator<Iterables, Iterators>, iter_tuple_common_ref_t<Iterators>,
                      fake_ptr_proxy<iter_tuple_common_ref_t<Iterators>>, iter_tuple_diff_type_t<Iterators>,
                      iter_tuple_iter_cat_t<Iterators>, default_sentinel_t> {

    Iterators _iterators{};
    Iterables _iterables{};

    using first_tuple_iterator = std::iterator_traits<first_it_t<Iterators>>;

    static constexpr size_t tup_size = tuple_size<Iterators>::value;

public:
    using value_type = typename first_tuple_iterator::value_type;
    using difference_type = iter_tuple_diff_type_t<Iterators>;
    using reference = iter_tuple_common_ref_t<Iterators>;
    using pointer = fake_ptr_proxy<reference>;

private:
    template<size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(index_sequence<I...>, const concatenate_iterator& other) const {
        using std::get;
        const difference_type totals[] = { static_cast<difference_type>(get<I>(_iterators) -
                                                                        get<I>(other._iterators))... };
        return std::accumulate(detail::begin(totals), detail::end(totals), difference_type{ 0 });
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(index_sequence<I...>) const {
        using std::get;
        const difference_type totals[] = { static_cast<difference_type>(get<I>(_iterators) -
                                                                        get<I>(_iterables).end())... };
        return std::accumulate(detail::begin(totals), detail::end(totals), difference_type{ 0 });
    }

#ifdef LZ_HAS_CXX_17

    template<size_t I>
    constexpr void min_is_n(const difference_type offset) {
        using std::get;
        if constexpr (I != 0) {
            auto& current = get<I>(_iterators);
            auto begin = get<I>(_iterables).begin();

            if (current != begin) {
                const auto dist = current - begin;
                if (dist <= offset) {
                    current = std::move(begin);
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
        else {
            LZ_ASSERT_SUBTRACTABLE(offset <= get<0>(_iterators) - get<0>(_iterables).begin());
            auto& current = get<0>(_iterators);
            current -= offset;
        }
    }

    template<size_t I>
    constexpr void plus_is_n(const difference_type offset) {
        using std::get;
        if constexpr (I != tup_size) {
            auto& current = get<I>(_iterators);
            const auto current_end = get<I>(_iterables).end();
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
        else {
            LZ_ASSERT_INCREMENTABLE(offset == 0);
        }
    }

    template<size_t I>
    constexpr void minus_minus() {
        using std::get;
        if constexpr (I != 0) {
            auto& current = get<I>(_iterators);
            if (current != get<I>(_iterables).begin()) {
                --current;
            }
            else {
                minus_minus<I - 1>();
            }
        }
        else {
            LZ_ASSERT_DECREMENTABLE(get<0>(_iterators) != get<0>(_iterables).begin());
            --get<0>(_iterators);
        }
    }

    template<size_t I>
    constexpr reference deref() const {
        using std::get;
        if constexpr (I == tup_size - 1) {
            LZ_ASSERT_DEREFERENCABLE(get<I>(_iterators) != get<I>(_iterables).end());
            return *get<I>(_iterators);
        }
        else {
            if (get<I>(_iterators) != get<I>(_iterables).end()) {
                return *get<I>(_iterators);
            }
            else {
                return deref<I + 1>();
            }
        }
    }

    template<size_t I>
    constexpr void plus_plus() {
        using std::get;
        if constexpr (I != tup_size) {
            if (get<I>(_iterators) != get<I>(_iterables).end()) {
                ++get<I>(_iterators);
            }
            else {
                plus_plus<I + 1>();
            }
        }
        else {
            LZ_ASSERT_INCREMENTABLE(get<tup_size - 1>(_iterators) != get<tup_size - 1>(_iterables).end());
        }
    }

    template<size_t I, class EndIter>
    constexpr bool iter_equal_to(const EndIter& end) const {
        using std::get;
        if constexpr (I != tup_size - 1) {
            const auto has_value = get<I>(_iterators) == get<I>(end);
            return has_value ? iter_equal_to<I + 1>(end) : has_value;
        }
        else {
            return get<I>(_iterators) == get<I>(end);
        }
    }

#else

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I != 0> min_is_n(const difference_type offset) {
        using std::get;
        auto& current = get<I>(_iterators);
        auto begin = get<I>(_iterables).begin();

        if (current != begin) {
            const auto dist = current - begin;
            if (dist <= offset) {
                current = std::move(begin);
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

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0> min_is_n(const difference_type offset) {
        using std::get;
        auto& current = get<0>(_iterators);
        LZ_ASSERT_SUBTRACTABLE(offset <= current - get<0>(_iterables).begin());
        current -= offset;
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I != tup_size> plus_is_n(const difference_type offset) {
        using std::get;
        auto& current = get<I>(_iterators);
        const auto& current_end = get<I>(_iterables).end();
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

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == tup_size> plus_is_n(const difference_type offset) const noexcept {
        LZ_ASSERT_ADDABLE(offset == 0);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I != 0> minus_minus() {
        using std::get;
        auto& current = get<I>(_iterators);
        if (current != get<I>(_iterables).begin()) {
            --current;
        }
        else {
            minus_minus<I - 1>();
        }
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0> minus_minus() {
        using std::get;
        LZ_ASSERT_DECREMENTABLE(get<0>(_iterables).begin() != get<0>(_iterables).end());
        --get<0>(_iterators);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == tup_size - 1, reference> deref() const {
        using std::get;
        return *get<I>(_iterators);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I != tup_size - 1, reference> deref() const {
        using std::get;
        if (get<I>(_iterators) != get<I>(_iterables).end()) {
            return *get<I>(_iterators);
        }
        else {
            return deref<I + 1>();
        }
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I != tuple_size<Iterators>::value> plus_plus() {
        using std::get;
        if (get<I>(_iterators) != get<I>(_iterables).end()) {
            ++get<I>(_iterators);
        }
        else {
            plus_plus<I + 1>();
        }
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == tuple_size<Iterators>::value> plus_plus() const noexcept {
    }

    template<size_t I, class EndIter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I != tuple_size<Iterators>::value - 1, bool> iter_equal_to(const EndIter& end) const {
        using std::get;
        const auto has_value = get<I>(_iterators) == get<I>(end);
        return has_value ? iter_equal_to<I + 1>(end) : has_value;
    }

    template<size_t I, class EndIter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == tuple_size<Iterators>::value - 1, bool> iter_equal_to(const EndIter& end) const {
        using std::get;
        return get<I>(_iterators) == get<I>(end);
    }

#endif // LZ_HAS_CXX_17

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

    constexpr concatenate_iterator()
        requires(std::default_initializable<Iterators> && std::default_initializable<Iterables>)
    = default;

#else

    template<class I = Iterators,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterables>::value>>
    constexpr concatenate_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<Iterables>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 concatenate_iterator(I&& iterables, Iterators iterators) :
        _iterators{ std::move(iterators) },
        _iterables{ std::forward<I>(iterables) } {
        static_assert(tup_size > 1, "Cannot concat one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 concatenate_iterator& operator=(default_sentinel_t) {
        assign_sentinels(make_index_sequence<tup_size>());
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return deref<0>();
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        plus_plus<0>();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        minus_minus<tup_size - 1>();
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        if (offset < 0) {
            min_is_n<tup_size - 1>(-offset);
        }
        else {
            plus_is_n<0>(offset);
        }
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const concatenate_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(begin_maybe_homo(_iterables) == begin_maybe_homo(other._iterables) &&
                             end_maybe_homo(_iterables) == end_maybe_homo(other._iterables));
        return minus(make_index_sequence<tup_size>(), other);
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(default_sentinel_t) const {
        return minus(make_index_sequence<tup_size>());
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const concatenate_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(begin_maybe_homo(_iterables) == begin_maybe_homo(other._iterables) &&
                             end_maybe_homo(_iterables) == end_maybe_homo(other._iterables));
        return iter_equal_to<0>(other._iterators);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel_t) const {
        return iter_equal_to<0>(end_maybe_homo(_iterables));
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_CONCATENATE_ITERATOR_HPP
