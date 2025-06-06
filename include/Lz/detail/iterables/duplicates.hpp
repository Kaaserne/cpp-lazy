#ifndef LZ_DUPLICATES_ITERABLE_HPP
#define LZ_DUPLICATES_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/duplicates.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable, class BinaryPredicate>
class duplicates_iterable : public lazy_view {
public:
    using iterator = duplicates_iterator<iter_t<Iterable>, sentinel_t<Iterable>, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    ref_or_view<Iterable> _iterable;
    func_container<BinaryPredicate> _compare;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr duplicates_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<BinaryPredicate>::value>>
    constexpr duplicates_iterable() {
    }

#endif

    template<class I>
    constexpr duplicates_iterable(I&& iterable, BinaryPredicate compare) :
        _iterable{ std::forward<I>(iterable) },
        _compare{ std::move(compare) } {
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return { detail::begin(_iterable), detail::begin(_iterable), detail::end(_iterable), _compare };
        }
        else {
            return { detail::begin(_iterable), detail::end(_iterable), _compare };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> begin() const& {
        return { detail::begin(_iterable), detail::begin(_iterable), detail::end(_iterable), _compare };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const& {
        return { detail::begin(_iterable), detail::end(_iterable), _compare };
    }

#endif // LZ_HAS_CXX_17

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_compare) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ detail::end(_iterable), detail::begin(_iterable), detail::end(_iterable), _compare };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { detail::end(_iterable), detail::begin(_iterable), detail::end(_iterable), _compare };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }

#endif // LZ_HAS_CXX_17
};
} // namespace detail
} // namespace lz

#endif
