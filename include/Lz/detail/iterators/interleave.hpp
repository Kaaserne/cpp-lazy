#pragma once

#include "Lz/detail/procs.hpp"
#include "Lz/detail/traits.hpp"
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

template<class IterTuple>
using first_iter = typename std::tuple_element<0, IterTuple>::type;

template<class I, class... Is>
struct all_same_reference;

template<class I, class... Is>
struct all_same_reference<std::tuple<I, Is...>> {
    static constexpr bool value = conjunction<std::is_same<ref_t<I>, ref_t<Is>>...>::value;
};

template<class IterTuple, class SentinelTuple>
class interleave_iterator
    : public iterator<
          interleave_iterator<IterTuple, SentinelTuple>, ref_t<first_iter<IterTuple>>,
          fake_ptr_proxy<ref_t<first_iter<IterTuple>>>, iter_tuple_diff_type_t<IterTuple>, iter_tuple_iter_cat_t<IterTuple>,
          sentinel_selector<iter_tuple_iter_cat_t<IterTuple>, interleave_iterator<IterTuple, SentinelTuple>, SentinelTuple>> {

    using traits = std::iterator_traits<first_iter<IterTuple>>;

public:
    using value_type = typename traits::value_type;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    static_assert(std::tuple_size<IterTuple>::value <= std::numeric_limits<std::uint_least8_t>::max(),
                  "interleave_iterator tuple size exceeds uint_least8_t. This is not supported.");

    static constexpr auto tuple_size = static_cast<std::uint_least8_t>(std::tuple_size<IterTuple>::value);

    IterTuple _iterators;
    // Using uint_least8_t because generally the max number of function parameters is 256
    std::uint_least8_t _index{};

    using index_sequence_for_this = make_index_sequence<tuple_size>;

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tuple_size - 1, bool> eq(const SentinelTuple& other) const {
        return std::get<I>(_iterators) == std::get<I>(other) ? _index == 0 : eq<I + 1>(other);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1, bool> eq(const SentinelTuple& other) const {
        return std::get<I>(_iterators) == std::get<I>(other);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tuple_size - 1, bool> eq(const interleave_iterator& other) const {
        return std::get<I>(_iterators) == std::get<I>(other._iterators) ? _index == other._index : eq<I + 1>(other);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1, bool> eq(const interleave_iterator& other) const {
        return std::get<I>(_iterators) == std::get<I>(other._iterators) && _index == other._index;
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<Is...>) {
        ++_index;
        if (_index != tuple_size) {
            return;
        }
        decompose(++std::get<Is>(_iterators)...);
        _index = 0;
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<Is...>) {
        if (_index == 0) {
            decompose(--std::get<Is>(_iterators)...);
            _index = tuple_size - 1;
            return;
        }
        --_index;
    }
    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tuple_size - 1, reference> dereference() const {
        return _index == I ? *std::get<I>(_iterators) : dereference<I + 1>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1, reference> dereference() const noexcept {
        LZ_ASSERT(_index == I, "Out of bounds dereference");
        return *std::get<I>(_iterators);
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 difference_type difference(const interleave_iterator& other, index_sequence<Is...>) const {
        const difference_type distances[] = { static_cast<difference_type>(std::get<Is>(_iterators) -
                                                                           std::get<Is>(other._iterators))... };
        const auto sum =
            std::accumulate(std::begin(distances), std::end(distances), difference_type{ 0 }, std::plus<difference_type>{});
        return sum + (static_cast<difference_type>(_index) - static_cast<difference_type>(other._index));
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n, index_sequence<Is...>) {
        if (n == 0) {
            return;
        }
        decompose(std::get<Is>(_iterators) += n...);
    }

public:
    LZ_CONSTEXPR_CXX_14 interleave_iterator(IterTuple iterators) : _iterators{ std::move(iterators) } {
        static_assert(tuple_size > 1, "interleaved_iterator must have at least two iterators");
        static_assert(all_same_reference<IterTuple>::value, "All iterators must have the same reference type");
    }

    LZ_CONSTEXPR_CXX_14 interleave_iterator& operator=(const SentinelTuple& end) {
        _iterators = end;
        _index = 0;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference<0>();
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(index_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        decrement(index_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const interleave_iterator& other) const {
        return difference(other, index_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(difference_type n) {
        if (n == 0) {
            return;
        }

        using unsigned_diff = typename std::make_unsigned<difference_type>::type;
        constexpr auto u_tup_size = static_cast<unsigned_diff>(tuple_size);
        const auto n_plus_index = static_cast<difference_type>(_index) + n;

        if (n > 0) {
            const auto u_n = static_cast<unsigned_diff>(n);
            plus_is(n_plus_index / static_cast<difference_type>(tuple_size), index_sequence_for_this{});
            _index = u_tup_size * (u_n / tuple_size) == u_n ? 0 : static_cast<std::uint_least8_t>(n_plus_index % u_tup_size);
            return;
        }

        const auto u_n = static_cast<unsigned_diff>(-n);
        constexpr auto s_tuple_size = static_cast<difference_type>(tuple_size);
        plus_is((n_plus_index - s_tuple_size + 1) / s_tuple_size, index_sequence_for_this{});
        // clang-format off
        _index = u_tup_size * (u_n / tuple_size) == u_n ? 
            0 : static_cast<std::uint_least8_t>((tuple_size + n_plus_index % s_tuple_size) % tuple_size);
        // clang-format on
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const interleave_iterator& other) const {
        return eq<0>(other);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const SentinelTuple& last) const {
        return eq<0>(last);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_INTERLEAVED_ITERATOR_HPP
