#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_EXCLUSIVE_SCAN_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class T, class BinaryOp>
class exclusive_scan_iterator : public iterator<exclusive_scan_iterator<Iterator, S, T, BinaryOp>, T&, fake_ptr_proxy<T&>,
                                                diff_type<Iterator>, std::forward_iterator_tag, default_sentinel_t> {
    Iterator _iterator;
    mutable T _reducer;
    S _end;
    bool _reached_end{ true };
    mutable BinaryOp _binary_op;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = T&;
    using value_type = decay_t<reference>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr exclusive_scan_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<S> && std::default_initializable<T> &&
                     std::default_initializable<BinaryOp>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value &&
                               std::is_default_constructible<T>::value && std::is_default_constructible<BinaryOp>::value>>
    constexpr exclusive_scan_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                                 std::is_nothrow_default_constructible<S>::value &&
                                                 std::is_nothrow_default_constructible<T>::value &&
                                                 std::is_nothrow_default_constructible<BinaryOp>::value) {
    }

#endif

    constexpr exclusive_scan_iterator(Iterator it, S end, T init, BinaryOp binary_op) :
        _iterator{ std::move(it) },
        _reducer{ std::move(init) },
        _end{ std::move(end) },
        _reached_end{ _iterator == _end },
        _binary_op{ std::move(binary_op) } {
    }

    LZ_CONSTEXPR_CXX_14 exclusive_scan_iterator& operator=(default_sentinel_t) {
        _iterator = _end;
        _reached_end = true;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT(!eq(lz::default_sentinel), "Cannot dereference end iterator");
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT(!eq(lz::default_sentinel), "Cannot increment end iterator");
        if (_iterator == _end) {
            _reached_end = true;
            return;
        }
        _reducer = _binary_op(std::move(_reducer), *_iterator);
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const exclusive_scan_iterator& b) const {
        LZ_ASSERT(_end == b._end, "Incompatible iterators");
        return _iterator == b._iterator && _reached_end == b._reached_end;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _end && _reached_end;
    }
};
} // namespace detail
} // namespace lz

#endif