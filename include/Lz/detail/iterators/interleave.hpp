#pragma once

#ifndef LZ_INTERLEAVED_ITERATOR_HPP
#define LZ_INTERLEAVED_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <numeric>

namespace lz {
namespace detail {

template<class IterMaybeHomo, class SMaybeHomo>
class interleave_iterator
    : public iterator<interleave_iterator<IterMaybeHomo, SMaybeHomo>, iter_tuple_common_ref_t<IterMaybeHomo>,
                      fake_ptr_proxy<iter_tuple_common_ref_t<IterMaybeHomo>>, iter_tuple_diff_type_t<IterMaybeHomo>,
                      iter_tuple_iter_cat_t<IterMaybeHomo>, SMaybeHomo> {

    using traits = std::iterator_traits<first_it_t<IterMaybeHomo>>;

public:
    using value_type = typename traits::value_type;
    using difference_type = iter_tuple_diff_type_t<IterMaybeHomo>;
    using reference = iter_tuple_common_ref_t<IterMaybeHomo>;
    using pointer = fake_ptr_proxy<reference>;

private:
    static_assert(tuple_size<IterMaybeHomo>::value <= std::numeric_limits<std::uint_least8_t>::max(),
                  "interleave_iterator tuple size exceeds uint_least8_t. This is not supported.");

    static constexpr auto tup_size = static_cast<std::uint_least8_t>(tuple_size<IterMaybeHomo>::value);

    IterMaybeHomo _iterators;
    // Using uint_least8_t because generally the max number of function parameters is 256
    std::uint_least8_t _index{};

    using is = make_index_sequence<tup_size>;

#ifdef LZ_HAS_CXX_17

    template<size_t I>
    constexpr bool eq(const SMaybeHomo& other) const {
        using std::get;
        if constexpr (I != tup_size - 1) {
            return get<I>(_iterators) == get<I>(other) ? _index == 0 : eq<I + 1>(other);
        }
        else {
            return get<I>(_iterators) == get<I>(other);
        }
    }

    template<size_t I>
    constexpr bool eq(const interleave_iterator& other) const {
        using std::get;
        if constexpr (I != tup_size - 1) {
            return get<I>(_iterators) == get<I>(other._iterators) ? _index == other._index : eq<I + 1>(other);
        }
        else {
            return get<I>(_iterators) == get<I>(other._iterators) && _index == other._index;
        }
    }

#else

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tup_size - 1, bool> eq(const SMaybeHomo& other) const {
        using std::get;
        return get<I>(_iterators) == get<I>(other) ? _index == 0 : eq<I + 1>(other);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tup_size - 1, bool> eq(const SMaybeHomo& other) const {
        using std::get;
        return get<I>(_iterators) == get<I>(other);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tup_size - 1, bool> eq(const interleave_iterator& other) const {
        using std::get;
        return get<I>(_iterators) == get<I>(other._iterators) ? _index == other._index : eq<I + 1>(other);
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tup_size - 1, bool> eq(const interleave_iterator& other) const {
        using std::get;
        return get<I>(_iterators) == get<I>(other._iterators) && _index == other._index;
    }

#endif

    template<size_t... Is>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<Is...>) {
        using std::get;
        ++_index;
        if (_index != tup_size) {
            return;
        }
#ifdef LZ_HAS_CXX_17
        (++get<Is>(_iterators), ...);
#else
        decompose(++get<Is>(_iterators)...);
#endif
        _index = 0;
    }

    template<size_t... Is>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<Is...>) {
        using std::get;
        if (_index == 0) {
#ifdef LZ_HAS_CXX_17
            (--get<Is>(_iterators), ...);
#else
            decompose(--get<Is>(_iterators)...);
#endif
            _index = tup_size - 1;
            return;
        }
        --_index;
    }

#ifdef LZ_HAS_CXX_17

    template<size_t I>
    constexpr reference dereference() const {
        using std::get;
        if constexpr (I != tup_size - 1) {
            return _index == I ? *get<I>(_iterators) : dereference<I + 1>();
        }
        else {
            return *get<I>(_iterators);
        }
    }

#else

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tup_size - 1, reference> dereference() const {
        using std::get;
        return _index == I ? *get<I>(_iterators) : dereference<I + 1>();
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tup_size - 1, reference> dereference() const noexcept {
        using std::get;
        return *get<I>(_iterators);
    }

#endif

    template<size_t... Is>
    LZ_CONSTEXPR_CXX_20 difference_type difference(const interleave_iterator& other, index_sequence<Is...>) const {
        using std::get;
        const difference_type distances[] = { static_cast<difference_type>(get<Is>(_iterators) -
                                                                           get<Is>(other._iterators))... };
        const auto sum =
            std::accumulate(std::begin(distances), std::end(distances), difference_type{ 0 }, std::plus<difference_type>{});
        return sum + (static_cast<difference_type>(_index) - static_cast<difference_type>(other._index));
    }

    template<size_t... Is>
    LZ_CONSTEXPR_CXX_20 difference_type difference(const SMaybeHomo& other, index_sequence<Is...>) const {
        using std::get;
        const difference_type distances[] = { static_cast<difference_type>(get<Is>(_iterators) - get<Is>(other))... };
        const auto sum = std::max({ distances[Is]... }) * static_cast<difference_type>(tup_size);
        return sum + static_cast<difference_type>(_index);
    }

    template<size_t... Is>
    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n, index_sequence<Is...>) {
        using std::get;
        if (n == 0) {
            return;
        }
#ifdef LZ_HAS_CXX_17
        ((get<Is>(_iterators) += n), ...);
#else
        decompose(get<Is>(_iterators) += n...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(const SMaybeHomo& end, index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) = get<I>(end)), ...);
#else
        decompose(get<I>(_iterators) = get<I>(end)...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr interleave_iterator()
        requires(std::default_initializable<IterMaybeHomo>)
    = default;

#else

    template<class I = IterMaybeHomo, class = enable_if<std::is_default_constructible<I>::value>>
    constexpr interleave_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 interleave_iterator(IterMaybeHomo iterators) : _iterators{ std::move(iterators) } {
        static_assert(tup_size > 1, "interleaved_iterator must have at least two iterators");
    }

    LZ_CONSTEXPR_CXX_14 interleave_iterator& operator=(const SMaybeHomo& end) {
        assign_sentinels(end, is{});
        _index = 0;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference<0>();
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(is{});
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        decrement(is{});
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const interleave_iterator& other) const {
        return difference(other, is{});
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const SMaybeHomo& other) const {
        return difference(other, is{});
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(difference_type n) {
        if (n == 0) {
            return;
        }

        using unsigned_diff = typename std::make_unsigned<difference_type>::type;
        constexpr auto u_tup_size = static_cast<unsigned_diff>(tup_size);
        const auto n_plus_index = static_cast<difference_type>(_index) + n;

        if (n > 0) {
            const auto u_n = static_cast<unsigned_diff>(n);
            plus_is(n_plus_index / static_cast<difference_type>(tup_size), is{});
            // clang-format off
            _index = u_tup_size * (u_n / tup_size) == u_n
                         ? 0 : static_cast<std::uint_least8_t>(static_cast<unsigned_diff>(n_plus_index) % u_tup_size);
            // clang-format on
            return;
        }

        const auto u_n = static_cast<unsigned_diff>(-n);
        constexpr auto s_tup_size = static_cast<difference_type>(tup_size);
        plus_is((n_plus_index - s_tup_size + 1) / s_tup_size, is{});
        // clang-format off
        _index = u_tup_size * (u_n / tup_size) == u_n ? 
            0 : static_cast<std::uint_least8_t>((tup_size + n_plus_index % s_tup_size) % tup_size);
        // clang-format on
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const interleave_iterator& other) const {
        return eq<0>(other);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const SMaybeHomo& last) const {
        return eq<0>(last);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_INTERLEAVED_ITERATOR_HPP
