#pragma once

#ifndef LZ_UNIQUE_ITERABLE_HPP
#define LZ_UNIQUE_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/unique.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable, class BinaryPredicate>
class unique_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    func_container<BinaryPredicate> _predicate;

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

public:
    using iterator = unique_iterator<maybe_owned<Iterable>, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<it, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr unique_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<BinaryPredicate>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value &&
                                                                std::is_default_constructible<BinaryPredicate>::value>>
    constexpr unique_iterable() noexcept(std::is_nothrow_default_constructible<maybe_owned<Iterable>>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I>
    constexpr unique_iterable(I&& iterable, BinaryPredicate compare) :
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::move(compare) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, _iterable.begin(), _predicate };
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr iterator begin() &&
        requires(return_sentinel)
    {
        return { _iterable, _iterable.begin(), std::move(_predicate) };
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, iterator> begin() && {
        return { _iterable, _iterable.begin(), std::move(_predicate) };
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _iterable.end(), _predicate };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return iterator{ _iterable, _iterable.end(), _predicate };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if_t<R, default_sentinel_t> end() const noexcept {
        return default_sentinel;
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_UNIQUE_ITERABLE_HPP
