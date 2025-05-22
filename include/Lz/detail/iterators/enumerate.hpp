#pragma once

#ifndef LZ_ENUMERATE_ITERATOR_HPP
#define LZ_ENUMERATE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class Arithmetic>
class enumerate_iterator
    : public iterator<enumerate_iterator<Iterator, S, Arithmetic>, std::pair<Arithmetic, ref_t<Iterator>>,
                      fake_ptr_proxy<std::pair<Arithmetic, ref_t<Iterator>>>, diff_type<Iterator>, iter_cat_t<Iterator>,
                      sentinel_selector<iter_cat_t<Iterator>, enumerate_iterator<Iterator, S, Arithmetic>, S>> {

    Iterator _iterator;
    Arithmetic _index{};

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = std::pair<Arithmetic, typename traits::value_type>;
    using reference = std::pair<Arithmetic, typename traits::reference>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

    constexpr enumerate_iterator(Iterator it, const Arithmetic start) : 
        _iterator{ std::move(it) },
        _index{ start } {
    }

    LZ_CONSTEXPR_CXX_14 enumerate_iterator& operator=(const S& end) {
        _iterator = end;
        _index = 0;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 enumerate_iterator& increment() {
        ++_index;
        ++_iterator;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 enumerate_iterator& decrement() {
        --_index;
        --_iterator;
        return *this;
    }

    constexpr reference dereference() const {
        return { _index, *_iterator };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const enumerate_iterator& other) const {
        return _iterator == other._iterator;
    }

    constexpr bool eq(const S& s) const {
        return _iterator == s;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
        _index += static_cast<Arithmetic>(n);
        _iterator += n;
    }

    constexpr difference_type difference(const enumerate_iterator& other) const {
        return _iterator - other._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif
