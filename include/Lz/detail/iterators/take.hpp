#pragma once

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<class Iterator, class S>
class take_iterator
    : public iterator<take_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, sentinel_selector<iter_cat_t<Iterator>, take_iterator<Iterator, S>, S>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator;

public:
    constexpr take_iterator() = default;

    constexpr take_iterator(Iterator it) : _iterator{ std::move(it) } {
    }

    LZ_CONSTEXPR_CXX_14 take_iterator& operator=(const S& end) {
        _iterator = end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        _iterator += offset;
    }

    constexpr difference_type difference(const take_iterator& b) const {
        return _iterator - b._iterator;
    }

    constexpr bool eq(const take_iterator& b) const {
        return _iterator == b._iterator;
    }

    constexpr bool eq(const S& sentinel) const {
        return _iterator == sentinel;
    }
};
} // namespace detail
} // namespace lz