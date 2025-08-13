#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_INCLUSIVE_SCAN_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class T, class BinaryOp>
class inclusive_scan_iterator : public iterator<inclusive_scan_iterator<Iterator, S, T, BinaryOp>, T&, fake_ptr_proxy<T&>,
                                                diff_type<Iterator>, std::forward_iterator_tag, default_sentinel_t> {
    Iterator _iterator;
    mutable T _reducer;
    S _end;
    mutable BinaryOp _binary_op;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = T&;
    using value_type = T;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = std::forward_iterator_tag;

#ifdef LZ_HAS_CONCEPTS

    constexpr inclusive_scan_iterator()
        requires(std::default_initializable<Iterator> && std::default_initializable<T> && std::default_initializable<BinaryOp> &&
                 std::default_initializable<S>)
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<T>::value &&
                               std::is_default_constructible<BinaryOp>::value && std::is_default_constructible<S>::value>>
    constexpr inclusive_scan_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                                 std::is_nothrow_default_constructible<T>::value &&
                                                 std::is_nothrow_default_constructible<BinaryOp>::value &&
                                                 std::is_nothrow_default_constructible<S>::value) {
    }

#endif

    constexpr inclusive_scan_iterator(Iterator it, S end, T init, BinaryOp bin_op) :
        _iterator{ std::move(it) },
        _reducer{ std::move(init) },
        _end{ std::move(end) },
        _binary_op{ std::move(bin_op) } {
    }

    LZ_CONSTEXPR_CXX_14 inclusive_scan_iterator& operator=(default_sentinel_t) {
        _iterator = _end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_iterator;
        if (_iterator != _end) {
            _reducer = _binary_op(std::move(_reducer), *_iterator);
        }
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const inclusive_scan_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_end == other._end);
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif