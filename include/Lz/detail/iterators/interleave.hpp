#pragma once

#ifndef LZ_INTERLEAVED_ITERATOR_HPP
#define LZ_INTERLEAVED_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<std::size_t TupleSize>
using tuple_of_bools = decltype(tuple_of_n<bool>(make_index_sequence<TupleSize>{}));

template<std::size_t N, class T>
using tuple_of_distances = decltype(tuple_of_n<T>(make_index_sequence<N>{}));

template<class IterTuple>
using first_iter = typename std::tuple_element<0, IterTuple>::type;

template<class IterTuple, class SentinelTuple, class = void>
class interleave_iterator;

template<class IterTuple, class SentinelTuple>
class interleave_iterator<IterTuple, SentinelTuple, enable_if<!is_bidi_tag<iter_tuple_iter_cat_t<IterTuple>>::value>>
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
    static constexpr std::size_t tuple_size = std::tuple_size<IterTuple>::value;

    IterTuple _iterators;
    tuple_of_bools<tuple_size> _is_next;

    using index_sequence_for_this = make_index_sequence<tuple_size>;

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I != tuple_size - 1, bool> is_any_turn() const {
        return std::get<I>(_is_next) || is_any_turn<I + 1>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1, bool> is_any_turn() const {
        return std::get<I>(_is_next);
    }

    template<std::size_t I, class EndIter>
    LZ_CONSTEXPR_CXX_14 enable_if<(I < tuple_size - 1), bool> eq(const EndIter& other) const {
        return std::get<I>(_iterators) == std::get<I>(other) ? !is_any_turn<I + 1>() : eq<I + 1>(other);
    }

    template<std::size_t I, class EndIter>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1, bool> eq(const EndIter& other) const {
        return std::get<I>(_iterators) == std::get<I>(other);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I < tuple_size - 1)> increment() {
        if (!std::get<I>(_is_next)) {
            increment<I + 1>();
            return;
        }

        ++std::get<I>(_iterators);
        std::get<I>(_is_next) = false;
        std::get<I + 1>(_is_next) = true;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1> increment() {
        ++std::get<I>(_iterators);
        std::get<I>(_is_next) = false;
        std::get<0>(_is_next) = true;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I < tuple_size), reference> dereference() const {
        return std::get<I>(_is_next) ? *std::get<I>(_iterators) : dereference<I + 1>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I == tuple_size), reference> dereference() const noexcept {
        LZ_ASSERT(I != tuple_size, "interleaved_iterator dereference out of bounds");
    }

public:
    LZ_CONSTEXPR_CXX_14 interleave_iterator(IterTuple iterators, const tuple_of_bools<tuple_size> is_next) :
        _iterators{ std::move(iterators) },
        _is_next{ is_next } {
        static_assert(tuple_size > 1, "interleaved_iterator must have at least two iterators");
    }

    constexpr interleave_iterator() = default;

    LZ_CONSTEXPR_CXX_14 interleave_iterator& operator=(const SentinelTuple& end) {
        _iterators = end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference<0>();
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment<0>();
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const interleave_iterator& other) const {
        return eq<0>(other._iterators);
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const SentinelTuple& last) const {
        return eq<0>(last);
    }
};

template<class IterTuple, class SentinelTuple>
class interleave_iterator<IterTuple, SentinelTuple, enable_if<is_bidi_tag<iter_tuple_iter_cat_t<IterTuple>>::value>>
    : public iterator<
          interleave_iterator<IterTuple, SentinelTuple>, ref_t<first_iter<IterTuple>>,
          fake_ptr_proxy<ref_t<first_iter<IterTuple>>>, iter_tuple_diff_type_t<IterTuple>, iter_tuple_iter_cat_t<IterTuple>,
          sentinel_selector<iter_tuple_iter_cat_t<IterTuple>, interleave_iterator<IterTuple, SentinelTuple>, SentinelTuple>> {

    static constexpr std::size_t tuple_size = std::tuple_size<IterTuple>::value;

    using traits = std::iterator_traits<first_iter<IterTuple>>;

public:
    using value_type = typename traits::value_type;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    IterTuple _iterators;
    tuple_of_distances<tuple_size, difference_type> _distances;
    tuple_of_bools<tuple_size> _is_next;

    template<std::size_t I, class EndIter>
    LZ_CONSTEXPR_CXX_14 enable_if<(I < tuple_size - 1), bool> eq(const EndIter& other) const {
        return std::get<I>(_iterators) == std::get<I>(other) ? !std::get<I + 1>(_is_next) : eq<I + 1>(other);
    }

    template<std::size_t I, class EndIter>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1, bool> eq(const EndIter& other) const {
        return std::get<I>(_iterators) == std::get<I>(other);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I < tuple_size - 1)> increment() {
        if (!std::get<I>(_is_next)) {
            increment<I + 1>();
            return;
        }

        ++std::get<I>(_iterators);
        ++std::get<I>(_distances);
        std::get<I>(_is_next) = false;
        std::get<I + 1>(_is_next) = true;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == tuple_size - 1> increment() {
        ++std::get<I>(_iterators);
        ++std::get<I>(_distances);
        std::get<I>(_is_next) = false;
        std::get<0>(_is_next) = true;
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I < tuple_size), reference> dereference() const {
        return std::get<I>(_is_next) ? *std::get<I>(_iterators) : dereference<I + 1>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<(I == tuple_size), reference> dereference() const noexcept {
        LZ_ASSERT(false, "interleaved_iterator dereference out of bounds");
    }

public:
    LZ_CONSTEXPR_CXX_14 interleave_iterator(IterTuple iterators, const tuple_of_bools<tuple_size> is_next,
                                            const tuple_of_distances<tuple_size, difference_type> distances) :
        _iterators{ std::move(iterators) },
        _distances{ distances },
        _is_next{ is_next } {
        static_assert(tuple_size > 1, "interleaved_iterator must have at least two iterators");
    }

    constexpr interleave_iterator() = default;

    LZ_CONSTEXPR_CXX_14 interleave_iterator& operator=(const SentinelTuple& end) {
        _iterators = end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference<0>();
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment<0>();
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const interleave_iterator& other) const {
        return eq<0>(other._iterators);
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const SentinelTuple& last) const {
        return eq<0>(last);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_INTERLEAVED_ITERATOR_HPP
