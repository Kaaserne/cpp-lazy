#pragma once

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<class Iterator>
class take_iterator
    : public iter_base<take_iterator<Iterator>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                       iter_cat_t<Iterator>, sentinel_selector<iter_cat_t<Iterator>, take_iterator<Iterator>>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator;
    difference_type _n;

public:
    constexpr take_iterator() = default;

    constexpr take_iterator(Iterator iterator, const difference_type start) noexcept :
        _iterator{ std::move(iterator) },
        _n{ start } {
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        --_n;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        ++_n;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        _iterator += offset;
        _n -= offset;
    }

    constexpr difference_type difference(const take_iterator& b) const {
        return _iterator - b._iterator;
    }

    constexpr bool eq(const take_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _n == 0;
    }
};

template<class Iterable>
class take_iterable {
    Iterable _iterable;
    diff_iterable_t<Iterable> _n;

public:
    using iterator = take_iterator<iter_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr take_iterable(I&& iterable, typename iterator::difference_type n) noexcept :
        _iterable{ std::forward<I>(iterable) },
        _n{ n } {
    }

    constexpr take_iterable() = default;

    LZ_NODISCARD constexpr std::size_t size() const noexcept {
        return static_cast<std::size_t>(_n);
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_ra<I>::value, iterator> begin() && {
        return { std::move(_iterable).begin(), _n };
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value, iterator> begin() && {
        return static_cast<const take_iterable&>(*this).begin();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _n };
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value, iterator> end() const& {
        return { std::begin(_iterable) + _n, 0 };
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD constexpr enable_if<!is_ra<I>::value, default_sentinel> end() const& noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz