#pragma once

#ifndef LZ_JOIN_WHERE_ITERATOR_HPP
#define LZ_JOIN_WHERE_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class IterA, class SA, class IterB, class SB, class SelectorA, class SelectorB, class ResultSelector>
class join_where_iterator : public iterator<join_where_iterator<IterA, SA, IterB, SB, SelectorA, SelectorB, ResultSelector>,
                                            func_ret_type_iters<ResultSelector, IterA, IterB>,
                                            fake_ptr_proxy<func_ret_type_iters<ResultSelector, IterA, IterB>>, std::ptrdiff_t,
                                            std::forward_iterator_tag, default_sentinel> {
private:
    using traits_a = std::iterator_traits<IterA>;
    using traits_b = std::iterator_traits<IterB>;
    using value_type_a = typename traits_a::value_type;
    using value_type_b = typename traits_b::value_type;
    using ref_type_a = typename traits_a::reference;

    using selector_a_ret_val = decay_t<func_ret_type<SelectorA, ref_type_a>>;

    basic_iterable<IterB, SB> _iterable;
    IterA _iter_a;
    IterB _begin_b;
    SA _end_a;

    mutable SelectorA _selector_a;
    mutable SelectorB _selector_b;
    mutable ResultSelector _result_selector;

    LZ_CONSTEXPR_CXX_17 void find_next() {
        using detail::find_if;
        using std::find_if;

        _iter_a = find_if(std::move(_iter_a), _end_a, [this](ref_t<IterA> a) {
            auto&& to_find = _selector_a(a);

            auto pos = lz::lower_bound(_iterable, to_find,
                                       [this](ref_t<IterB> b, const selector_a_ret_val& val) { return _selector_b(b) < val; });

            if (pos != _iterable.end() && !(to_find < _selector_b(*pos))) {
                _iterable = lz::basic_iterable<IterB, SB>{ pos, _iterable.end() };
                return true;
            }
            _iterable = lz::basic_iterable<IterB, SB>{ _begin_b, _iterable.end() };
            return false;
        });
    }

public:
    using reference = decltype(_result_selector(*_iter_a, *_iterable.begin()));
    using value_type = decay_t<reference>;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr join_where_iterator()
        requires std::default_initializable<IterA> && std::default_initializable<SA> && std::default_initializable<IterB> &&
                     std::default_initializable<SB> && std::default_initializable<SelectorA> &&
                     std::default_initializable<SelectorB> && std::default_initializable<ResultSelector>
    = default;

#else

    template<
        class I = IterA,
        class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<SA>::value &&
                          std::is_default_constructible<IterB>::value && std::is_default_constructible<SB>::value &&
                          std::is_default_constructible<SelectorA>::value && std::is_default_constructible<SelectorB>::value &&
                          std::is_default_constructible<ResultSelector>::value>>
    constexpr join_where_iterator() {
    }

#endif

    LZ_CONSTEXPR_CXX_17
    join_where_iterator(IterA it_a, SA end_a, IterB it_b, SB end_b, SelectorA a, SelectorB b, ResultSelector result_selector) :
        _iterable{ std::move(it_b), std::move(end_b) },
        _iter_a{ std::move(it_a) },
        _begin_b{ _iterable.begin() },
        _end_a{ std::move(end_a) },
        _selector_a{ std::move(a) },
        _selector_b{ std::move(b) },
        _result_selector{ std::move(result_selector) } {
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 join_where_iterator& operator=(default_sentinel) {
        _iter_a = _end_a;
        return *this;
    }

    constexpr reference dereference() const {
        return _result_selector(*_iter_a, *_iterable.begin());
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _iterable = lz::basic_iterable<IterB, SB>(std::next(_iterable.begin()), _iterable.end());
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const join_where_iterator& b) const {
        LZ_ASSERT(_end_a == b._end_a, "Incompatible iterators");
        return _iter_a == b._iter_a;
    }

    constexpr bool eq(default_sentinel) const {
        return _iter_a == _end_a;
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_JOIN_WHERE_ITERATOR_HPP
