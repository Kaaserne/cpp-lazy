#pragma once

#ifndef LZ_DETAIL_COMMON_ITERATOR_HPP
#define LZ_DETAIL_COMMON_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/variant.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterator, class S>
class common_iterator : public iterator<common_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                                        diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel> {
    variant<Iterator, S> _data;

    using traits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename traits::iterator_category;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr common_iterator() noexcept = default;

    constexpr common_iterator(const Iterator& iter) : _data{ iter } {
    }

    constexpr common_iterator(Iterator&& iter) noexcept : _data{ std::move(iter) } {
    }

    constexpr common_iterator(const S& sent) : _data{ sent } {
    }

    constexpr common_iterator(S&& sent) noexcept : _data{ std::move(sent) } {
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(const Iterator& iter) {
        _data = iter;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(const S& sent) {
        _data = sent;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(Iterator&& iter) noexcept {
        _data = std::move(iter);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(S&& sent) noexcept {
        _data = std::move(sent);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT(_data.index() == 0, "Dereferencing a sentinel/end");
#ifdef __cpp_lib_variant
        using std::get;
#endif
        return *get<0>(_data);
    }

    LZ_CONSTEXPR_CXX_14 fake_ptr_proxy<reference> arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT(_data.index() == 0, "Incrementing a sentinel/past the end");
#ifdef __cpp_lib_variant
        using std::get;
#endif
        ++get<0>(_data);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const common_iterator& rhs) const {
#ifdef __cpp_lib_variant
        using std::get;
#endif
        if (_data.index() != rhs._data.index()) {
            return _data.index() == 0 ? get<0>(_data) == get<1>(rhs._data) : get<1>(_data) == get<0>(rhs._data);
        }
        return true;
    }
};
} // namespace detail
} // namespace lz

#endif