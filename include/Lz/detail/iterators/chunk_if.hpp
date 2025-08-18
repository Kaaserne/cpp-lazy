#pragma once

#ifndef LZ_CHUNK_IF_ITERATOR_HPP
#define LZ_CHUNK_IF_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>

namespace lz {
namespace detail {
template<class ValueType, class Iterator, class S, class UnaryPredicate>
class chunk_if_iterator
    : public iterator<chunk_if_iterator<ValueType, Iterator, S, UnaryPredicate>, ValueType, fake_ptr_proxy<ValueType>,
                      diff_type<Iterator>, std::forward_iterator_tag, default_sentinel_t> {
    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = ValueType;
    using difference_type = typename iter_traits::difference_type;
    using reference = value_type;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _sub_range_begin;
    Iterator _sub_range_end;
    bool _ends_with_trailing{ true };
    S _end;
    mutable UnaryPredicate _predicate;

    LZ_CONSTEXPR_CXX_14 void find_next() {
        using detail::find_if;
        using std::find_if;
        _sub_range_end = find_if(_sub_range_end, _end, _predicate);
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr chunk_if_iterator()
        requires(std::default_initializable<Iterator> && std::default_initializable<S> &&
                 std::default_initializable<UnaryPredicate>)
    = default;

#else

    template<class I = Iterator,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value &&
                                 std::is_default_constructible<UnaryPredicate>::value>>
    constexpr chunk_if_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                           std::is_nothrow_default_constructible<S>::value &&
                                           std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 chunk_if_iterator(Iterator begin, S end, UnaryPredicate predicate, bool is_empty) :
        _sub_range_begin{ std::move(begin) },
        _sub_range_end{ _sub_range_begin },
        _end{ std::move(end) },
        _predicate{ std::move(predicate) } {
        if (_sub_range_begin != _end) {
            find_next();
        }
        if (is_empty) {
            _ends_with_trailing = false;
        }
    }

    LZ_CONSTEXPR_CXX_14 chunk_if_iterator& operator=(default_sentinel_t) {
        _sub_range_begin = _end;
        _ends_with_trailing = false;
        return *this;
    }

#ifdef LZ_HAS_CXX_17

    constexpr reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        if constexpr (std::is_constructible_v<ValueType, Iterator, Iterator>) {
            return { _sub_range_begin, _sub_range_end };
        }
        else {
            return { detail::addressof(*_sub_range_begin), static_cast<size_t>(_sub_range_end - _sub_range_begin) };
        }
    }

#else

    template<class V = ValueType>
    LZ_CONSTEXPR_CXX_14 enable_if_t<std::is_constructible<V, Iterator, Iterator>::value, reference> dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { _sub_range_begin, _sub_range_end };
    }

    // Overload for std::string, [std/lz]::string_view
    template<class V = ValueType>
    LZ_CONSTEXPR_CXX_14 enable_if_t<!std::is_constructible<V, Iterator, Iterator>::value, reference> dereference() const {
        static_assert(is_ra<Iterator>::value, "Iterator must be a random access");
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { detail::addressof(*_sub_range_begin), static_cast<size_t>(_sub_range_end - _sub_range_begin) };
    }

#endif

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        if (_ends_with_trailing && _sub_range_end == _end) {
            _sub_range_begin = _sub_range_end;
            _ends_with_trailing = false;
            return;
        }

        auto prev = _sub_range_end++;
        if (_sub_range_end != _end) {
            _sub_range_begin = _sub_range_end;
            find_next();
            return;
        }

        if (_predicate(*prev)) {
            _sub_range_begin = _sub_range_end = _ends_with_trailing ? std::move(prev) : _sub_range_end;
            _ends_with_trailing = false;
            return;
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const chunk_if_iterator& rhs) const {
        LZ_ASSERT_COMPATIBLE(_end == rhs._end);
        return _sub_range_begin == rhs._sub_range_begin && _sub_range_end == rhs._sub_range_end &&
               _ends_with_trailing == rhs._ends_with_trailing;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _sub_range_begin == _end && !_ends_with_trailing;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNK_IF_ITERATOR_HPP
