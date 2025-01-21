#pragma once

#ifndef LZ_CHUNK_IF_ITERATOR_HPP
#define LZ_CHUNK_IF_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/iterator_base.hpp>

// TODO add benchmarks for maybe_no_unique_address
/*
// Wrapper to conditionally apply [[no_unique_address]]
template <typename T, bool = std::is_empty_v<T>>
struct maybe_no_unique_address;

// Specialization when T is empty
template <typename T>
struct maybe_no_unique_address<T, true> : private T {
    maybe_no_unique_address() = default;
    maybe_no_unique_address(const T& t) : T(t) {}

    T& get() { return *this; }
    const T& get() const { return *this; }
};

// Specialization when T is not empty
template <typename T>
struct maybe_no_unique_address<T, false> {
    [[no_unique_address]] T value;
    maybe_no_unique_address() = default;
    maybe_no_unique_address(const T& t) : value(t) {}

    T& get() { return value; }
    const T& get() const { return value; }
};
*/
namespace lz {
namespace detail {
template<class ValueType, class Iterator, class S, class UnaryPredicate>
class chunk_if_iterator
    : public iter_base<chunk_if_iterator<ValueType, Iterator, S, UnaryPredicate>, ValueType, fake_ptr_proxy<ValueType>,
                       diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {
    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = ValueType;
    using difference_type = typename iter_traits::difference_type;
    using reference = value_type;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _sub_range_begin;
    Iterator _sub_range_end;
    bool _trailing_empty{ true };
    S _end;
    mutable func_container<UnaryPredicate> _predicate;

    LZ_CONSTEXPR_CXX_14 void find_next() {
        using detail::find_if;
        using std::find_if;
        _sub_range_end = find_if(_sub_range_end, _end, _predicate);
    }

public:
    constexpr chunk_if_iterator() = default;

    LZ_CONSTEXPR_CXX_14 chunk_if_iterator(Iterator begin, S end, UnaryPredicate predicate, bool is_empty) :
        _sub_range_begin{ std::move(begin) },
        _sub_range_end{ _sub_range_begin },
        _end{ std::move(end) },
        _predicate{ std::move(predicate) } {
        if (_sub_range_begin != _end) {
            find_next();
        }
        if (is_empty) {
            _trailing_empty = false;
        }
    }

    template<class V = ValueType>
    constexpr enable_if<std::is_constructible<V, Iterator, Iterator>::value, reference> dereference() const {
        return { _sub_range_begin, _sub_range_end };
    }

    // Overload for std::string, [std/lz]::string_view
    template<class V = ValueType>
    constexpr enable_if<!std::is_constructible<V, Iterator, Iterator>::value, reference> dereference() const {
        static_assert(!is_ra_tag<Iterator>::value, "Iterator must be a random access range");
        return { std::addressof(*_sub_range_begin), static_cast<std::size_t>(_sub_range_end - _sub_range_begin) };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        if (_trailing_empty && _sub_range_end == _end) {
            _sub_range_begin = _sub_range_end;
            _trailing_empty = false;
            return;
        }

        auto prev = _sub_range_end++;
        if (_sub_range_end != _end) {
            _sub_range_begin = _sub_range_end;
            find_next();
            return;
        }

        if (_predicate(*prev)) {
            _sub_range_begin = _sub_range_end = _trailing_empty ? std::move(prev) : _sub_range_end;
            _trailing_empty = false;
            return;
        }
    }

    constexpr bool eq(const chunk_if_iterator& rhs) const {
        return _sub_range_begin == rhs._sub_range_begin && _sub_range_end == rhs._sub_range_end &&
               _trailing_empty == rhs._trailing_empty;
    }

    constexpr bool eq(default_sentinel) const {
        return _sub_range_begin == _end && !_trailing_empty;
    }
};

template<class ValueType, class Iterable, class UnaryPredicate>
class chunk_if_iterable {
    Iterable _iterable;
    UnaryPredicate _predicate;

public:
    using iterator = chunk_if_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I, class Up>
    constexpr chunk_if_iterable(I&& iterable, Up&& predicate) :
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::forward<Up>(predicate) } {
    }

    constexpr chunk_if_iterable() = default;

    iterator begin() && {
        auto begin = std::move(_iterable).begin();
        auto end = std::move(_iterable).end();
        const auto is_end = end == begin;
        return { begin, end, is_end };
    }

    iterator begin() const& {
        const auto is_end = std::end(_iterable) == std::begin(_iterable);
        return { std::begin(_iterable), std::end(_iterable), _predicate, is_end };
    }

    constexpr default_sentinel end() const& {
        return {};
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_CHUNK_IF_ITERATOR_HPP
