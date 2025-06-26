#pragma once

#ifndef LZ_DROP_ITERABLE_HPP
#define LZ_DROP_ITERABLE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class drop_iterable : public lazy_view {
public:
    using iterator = iter_t<Iterable>;
    using sentinel = sentinel_t<Iterable>;
    using const_iterator = iterator;
    using value_type = val_iterable_t<Iterable>;

private:
    using diff = diff_type<iterator>;

    ref_or_view<Iterable> _iterable;
    std::size_t _n{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr drop_iterable()
        requires std::default_initializable<ref_or_view<Iterable>>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr drop_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr drop_iterable(I&& iterable, const std::size_t n) : _iterable{ std::forward<I>(iterable) }, _n{ n } {
    }

    template<bool Sized = sized<Iterable>::value>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<Sized, std::size_t> size() const {
        const auto size = lz::size(_iterable);
        return size > _n ? size - _n : 0;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return next_fast(std::move(_iterable), static_cast<diff>(_n));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return next_fast(_iterable, static_cast<diff>(_n));
    }

    LZ_NODISCARD constexpr sentinel end() const& {
        return std::end(_iterable);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel end() && {
        return detail::end(std::move(_iterable));
    }
};
} // namespace detail
} // namespace lz

#endif