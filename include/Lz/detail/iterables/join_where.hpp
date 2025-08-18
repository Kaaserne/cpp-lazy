#pragma once

#ifndef LZ_JOIN_WHERE_ITERABLE_HPP
#define LZ_JOIN_WHERE_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/join_where.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
class join_where_iterable : public lazy_view {
    maybe_owned<IterableA> _iterable_a;
    maybe_owned<IterableB> _iterable_b;
    func_container<SelectorA> _a;
    func_container<SelectorB> _b;
    func_container<ResultSelector> _result_selector;

public:
    using iterator = join_where_iterator<maybe_owned<IterableA>, iter_t<IterableB>, sentinel_t<IterableB>,
                                         func_container<SelectorA>, func_container<SelectorB>, func_container<ResultSelector>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !has_sentinel<IterableA>::value || !is_bidi_tag<typename iterator::iterator_category>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr join_where_iterable()
        requires(std::default_initializable<IterableA> && std::default_initializable<IterableB> &&
                 std::default_initializable<SelectorA> && std::default_initializable<SelectorB> &&
                 std::default_initializable<ResultSelector>)
    = default;

#else

    template<
        class I = decltype(_iterable_a),
        class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<IterableB>::value &&
                          std::is_default_constructible<SelectorA>::value && std::is_default_constructible<SelectorB>::value &&
                          std::is_default_constructible<ResultSelector>::value>>
    constexpr join_where_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<maybe_owned<IterableB>>::value &&
                                             std::is_nothrow_default_constructible<SelectorA>::value &&
                                             std::is_nothrow_default_constructible<SelectorB>::value &&
                                             std::is_nothrow_default_constructible<ResultSelector>::value) {
    }

#endif

    template<class I, class I2>
    constexpr join_where_iterable(I&& iterable, I2&& iterable2, SelectorA a, SelectorB b, ResultSelector result_selector) :
        _iterable_a{ std::forward<I>(iterable) },
        _iterable_b{ std::forward<I2>(iterable2) },
        _a{ std::move(a) },
        _b{ std::move(b) },
        _result_selector{ std::move(result_selector) } {
    }

    constexpr iterator begin() const& {
        return { _iterable_a, _iterable_a.begin(), _iterable_b.begin(), _iterable_b.end(), _a, _b, _result_selector };
    }

    LZ_CONSTEXPR_CXX_14 iterator begin() && {
        // clang-format off
        return { _iterable_a, _iterable_a.begin(), detail::begin(std::move(_iterable_b)),
                 detail::end(std::move(_iterable_b)), std::move(_a), std::move(_b), std::move(_result_selector) };
        // clang-format on
    }

    LZ_NODISCARD constexpr default_sentinel_t end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_JOIN_WHERE_ITERABLE_HPP
