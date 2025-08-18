#ifndef LZ_DUPLICATES_ITERABLE_HPP
#define LZ_DUPLICATES_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/duplicates.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable, class BinaryPredicate>
class duplicates_iterable : public lazy_view {
public:
    using iterator = duplicates_iterator<maybe_owned<Iterable>, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

private:
    maybe_owned<Iterable> _iterable;
    func_container<BinaryPredicate> _compare;

    static constexpr bool return_sentinel = !is_bidi_tag<iter_cat_t<iterator>>::value || has_sentinel<Iterable>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr duplicates_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<BinaryPredicate>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value &&
                                                                std::is_default_constructible<BinaryPredicate>::value>>
    constexpr duplicates_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I>
    constexpr duplicates_iterable(I&& iterable, BinaryPredicate compare) :
        _iterable{ std::forward<I>(iterable) },
        _compare{ std::move(compare) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, _iterable.begin(), _compare };
    }

#ifdef LZ_HAS_CONCEPTS

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() &&
        requires(return_sentinel)
    {
        return { _iterable, _iterable.begin(), std::move(_compare) };
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, iterator> begin() && {
        return { _iterable, _iterable.begin(), std::move(_compare) };
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _iterable.end(), _compare };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterable, _iterable.end(), _compare };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if_t<R, default_sentinel_t> end() const {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif
