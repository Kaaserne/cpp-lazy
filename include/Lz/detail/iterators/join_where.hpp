#pragma once

#ifndef LZ_JOIN_WHERE_ITERATOR_HPP
#define LZ_JOIN_WHERE_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class IterableA, class IterB, class SB, class SelectorA, class SelectorB, class ResultSelector>
class join_where_iterator
    : public iterator<join_where_iterator<IterableA, IterB, SB, SelectorA, SelectorB, ResultSelector>,
                      func_ret_type_iters<ResultSelector, iter_t<IterableA>, IterB>,
                      fake_ptr_proxy<func_ret_type_iters<ResultSelector, iter_t<IterableA>, IterB>>, std::ptrdiff_t,
                      common_type<std::bidirectional_iterator_tag, iter_cat_t<iter_t<IterableA>>>, default_sentinel_t> {
private:
    using iter_a = iter_t<IterableA>;
    using traits_a = std::iterator_traits<iter_a>;
    using traits_b = std::iterator_traits<IterB>;
    using value_type_a = typename traits_a::value_type;
    using value_type_b = typename traits_b::value_type;
    using ref_type_a = typename traits_a::reference;

    using selector_a_ret_val = remove_cvref<func_ret_type<SelectorA, ref_type_a>>;

    basic_iterable<IterB, SB> _iterable_b;
    iter_a _iter_a;
    IterB _begin_b;
    IterableA _iterable_a;

    mutable SelectorA _selector_a;
    mutable SelectorB _selector_b;
    mutable ResultSelector _result_selector;

    LZ_CONSTEXPR_CXX_17 void find_next() {
        using detail::find_if;
        using std::find_if;

        _iter_a = find_if(std::move(_iter_a), _iterable_a.end(), [this](ref_t<iter_a> a) {
            auto&& to_find = _selector_a(a);

            auto pos = lz::lower_bound(_iterable_b, to_find,
                                       [this](ref_t<IterB> b, const selector_a_ret_val& val) { return _selector_b(b) < val; });

            if (pos != _iterable_b.end() && !(to_find < _selector_b(*pos))) {
                _iterable_b = lz::basic_iterable<IterB, SB>{ std::move(pos), _iterable_b.end() };
                return true;
            }
            _iterable_b = lz::basic_iterable<IterB, SB>{ _begin_b, _iterable_b.end() };
            return false;
        });
    }

public:
    using reference = decltype(_result_selector(*_iter_a, *_iterable_b.begin()));
    using value_type = remove_cvref<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr join_where_iterator()
        requires(std::default_initializable<IterableA> && std::default_initializable<iter_a> &&
                 std::default_initializable<IterB> && std::default_initializable<SB> && std::default_initializable<SelectorA> &&
                 std::default_initializable<SelectorB> && std::default_initializable<ResultSelector>)
    = default;

#else

    template<
        class I = iter_a,
        class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<IterableA>::value &&
                          std::is_default_constructible<IterB>::value && std::is_default_constructible<SB>::value &&
                          std::is_default_constructible<SelectorA>::value && std::is_default_constructible<SelectorB>::value &&
                          std::is_default_constructible<ResultSelector>::value>>
    constexpr join_where_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<IterableA>::value &&
                                             std::is_nothrow_default_constructible<IterB>::value &&
                                             std::is_nothrow_default_constructible<SB>::value &&
                                             std::is_nothrow_default_constructible<SelectorA>::value &&
                                             std::is_nothrow_default_constructible<SelectorB>::value &&
                                             std::is_nothrow_default_constructible<ResultSelector>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_17 join_where_iterator(I&& iterable, iter_a it_a, IterB it_b, SB end_b, SelectorA a, SelectorB b,
                                            ResultSelector result_selector) :
        _iterable_b{ std::move(it_b), std::move(end_b) },
        _iter_a{ std::move(it_a) },
        _begin_b{ _iterable_b.begin() },
        _iterable_a{ std::forward<I>(iterable) },
        _selector_a{ std::move(a) },
        _selector_b{ std::move(b) },
        _result_selector{ std::move(result_selector) } {
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 join_where_iterator& operator=(default_sentinel_t) {
        _iter_a = _iterable_a.end();
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return _result_selector(*_iter_a, *_iterable_b.begin());
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        _iterable_b = lz::basic_iterable<IterB, SB>{ std::next(_iterable_b.begin()), _iterable_b.end() };
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const join_where_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable_a.end() == other._iterable_a.end());
        return _iter_a == other._iter_a;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iter_a == _iterable_a.end();
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_JOIN_WHERE_ITERATOR_HPP
