#pragma once

#ifndef LZ_GROUP_BY_ITERABLE_HPP
#define LZ_GROUP_BY_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/group_by.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class Iterable, class BinaryPredicate>
class group_by_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    func_container<BinaryPredicate> _binary_predicate;

public:
    using iterator = group_by_iterator<maybe_owned<Iterable>, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<iter_t<Iterable>, sentinel_t<Iterable>>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr group_by_iterable()
        requires std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<BinaryPredicate>::value>>
    constexpr group_by_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                           std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 group_by_iterable(I&& iterable, BinaryPredicate binary_predicate) :
        _iterable{ std::forward<I>(iterable) },
        _binary_predicate{ std::move(binary_predicate) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, std::begin(_iterable), _binary_predicate };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, iterator> begin() && {
        return { _iterable, std::begin(_iterable), std::move(_binary_predicate) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, std::end(_iterable), _binary_predicate };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const {
        return { _iterable, std::end(_iterable), _binary_predicate };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, default_sentinel_t> end() const {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_GROUP_BY_ITERABLE_HPP