#pragma once

#ifndef LZ_SPLIT_ITERATOR_HPP
#define LZ_SPLIT_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class ValueType, class Iterator, class S, class Iterator2, class S2>
class split_iterator : public iter_base<split_iterator<ValueType, Iterator, S, Iterator2, S2>, ValueType,
                                        fake_ptr_proxy<ValueType>, std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {
    std::pair<Iterator, Iterator> _sub_range_end;
    Iterator _sub_range_begin;
    Iterator2 _to_search;
    S _end;
    S2 _to_search_end;
    bool _trailingEmpty{ true };

    LZ_CONSTEXPR_CXX_14 std::pair<Iterator, Iterator> search() const {
        using std::search;
        return search(_sub_range_end.second, _end, _to_search, _to_search_end, MAKE_BIN_PRED(equal_to){});
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = ValueType;
    using reference = value_type;
    using difference_type = diff_type<Iterator>;
    using pointer = fake_ptr_proxy<reference>;

    constexpr split_iterator() = default;

    LZ_CONSTEXPR_CXX_14 split_iterator(Iterator begin, S end, Iterator2 begin2, S2 end2) :
        _sub_range_end{ begin, begin },
        _sub_range_begin{ std::move(begin) },
        _to_search{ std::move(begin2) },
        _end{ std::move(end) },
        _to_search_end{ std::move(end2) } {
        if (_sub_range_begin != _end) {
            _sub_range_end = search();
        }
        else {
            _trailingEmpty = false;
        }
    }

    template<class V = ValueType>
    constexpr enable_if<std::is_constructible<V, Iterator, Iterator>::value, reference> dereference() const {
        return { _sub_range_begin, _sub_range_end.first };
    }

    // Overload for std::string, [std/lz]::string_view
    template<class V = ValueType>
    LZ_CONSTEXPR_CXX_17 enable_if<!std::is_constructible<V, Iterator, Iterator>::value, reference> dereference() const {
        static_assert(is_ra<Iterator>::value, "Iterator must be a random access");
        return { std::addressof(*_sub_range_begin), static_cast<std::size_t>(_sub_range_end.first - _sub_range_begin) };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        if (_trailingEmpty && _sub_range_end.second == _end) {
            _sub_range_begin = _sub_range_end.first;
            _trailingEmpty = false;
            return;
        }

        if (!_trailingEmpty && _sub_range_end.second == _end) {
            _sub_range_begin = _sub_range_end.first = _sub_range_end.second;
            return;
        }

        _sub_range_end.first = _sub_range_end.second;
        if (_sub_range_end.first != _end) {
            _sub_range_begin = _sub_range_end.first;
            _sub_range_end = search();
        }
    }

    constexpr bool eq(const split_iterator& rhs) const noexcept {
        return _sub_range_begin == rhs._sub_range_begin && _sub_range_end.first == rhs._sub_range_end.first &&
               _trailingEmpty == rhs._trailingEmpty;
    }

    constexpr bool eq(default_sentinel) const {
        return _sub_range_begin == _end && !_trailingEmpty;
    }
};
} // namespace detail
} // namespace lz

#endif