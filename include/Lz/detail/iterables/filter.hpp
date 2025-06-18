#pragma once

#ifndef LZ_FILTER_ITERABLE_HPP
#define LZ_FILTER_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/filter.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryPredicate>
class filter_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    func_container<UnaryPredicate> _predicate;

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

public:
    using iterator = filter_iterator<ref_or_view<Iterable>, func_container<UnaryPredicate>>;
    using const_iterator = iterator;
    using sentinel = typename iterator::sentinel;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<it, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr filter_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<UnaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryPredicate>::value>>
    constexpr filter_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    template<class I>
    constexpr filter_iterable(I&& iterable, UnaryPredicate predicate) :
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::move(predicate) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, std::begin(_iterable), _predicate };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, iterator> begin() && {
        return { _iterable, std::begin(_iterable), std::move(_predicate) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, std::end(_iterable), _predicate };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const {
        return { _iterable, std::end(_iterable), _predicate };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if<R, default_sentinel> end() const noexcept {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // end LZ_FILTER_ITERABLE_HPP