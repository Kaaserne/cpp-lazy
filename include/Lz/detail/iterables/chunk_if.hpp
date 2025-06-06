#pragma once

#ifndef LZ_CHUNK_IF_ITERABLE_HPP
#define LZ_CHUNK_IF_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/chunk_if.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class ValueType, class Iterable, class UnaryPredicate>
class chunk_if_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    func_container<UnaryPredicate> _predicate;

public:
    using iterator = chunk_if_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, func_container<UnaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr chunk_if_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<UnaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryPredicate>::value>>
    constexpr chunk_if_iterable() {
    }

#endif

    template<class I>
    constexpr chunk_if_iterable(I&& iterable, UnaryPredicate predicate) :
        _iterable{ iterable },
        _predicate{ std::move(predicate) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        auto begin = detail::begin(std::move(_iterable));
        auto end = detail::end(std::move(_iterable));
        const auto is_end = end == begin;
        return { begin, end, std::move(_predicate), is_end };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        const auto is_end = std::end(_iterable) == std::begin(_iterable);
        return { std::begin(_iterable), std::end(_iterable), _predicate, is_end };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif