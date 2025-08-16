#pragma once

#ifndef LZ_PAIRWISE_ITERABLE_HPP
#define LZ_PAIRWISE_ITERABLE_HPP

#include <Lz/detail/iterators/pairwise.hpp>
#include <Lz/detail/maybe_owned.hpp>
// TODO make benchmarks and examples
namespace lz {
namespace detail {

template<class Iterable>
class pairwise_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    std::size_t _pair_size{};

public:
    using iterator = conditional<!is_ra<iter_t<Iterable>>::value, bidi_pairwise_iterator<maybe_owned<Iterable>>,
                                 ra_pairwise_iterator<maybe_owned<Iterable>>>;
    using value_type = typename iterator::value_type;
    using const_iterator = iterator;

private:
    static constexpr auto return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || has_sentinel<Iterable>::value;

    using diff = typename iterator::difference_type;
    using it_cat = typename iterator::iterator_category;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr pairwise_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr pairwise_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 pairwise_iterable(I&& iterable, const std::size_t pair_size) :
        _iterable{ std::forward<I>(iterable) },
        _pair_size{ pair_size } {
        LZ_ASSERT(_pair_size != 0, "Size must be greater than zero");
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr std::size_t size() const
        requires(sized<Iterable>)
    {
        const auto it_size = lz::size(_iterable);
        return it_size < _pair_size ? 0 : it_size - _pair_size + 1;
    }

#else

    template<bool S = is_sized<Iterable>::value>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<S, std::size_t> size() const {
        const auto it_size = lz::size(_iterable);
        return it_size < _pair_size ? 0 : it_size - _pair_size + 1;
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const {
        if constexpr (is_ra_tag_v<it_cat>) {
            return { _iterable, _iterable.begin(), _pair_size };
        }
        else if constexpr (is_sized_v<Iterable> && !has_sentinel_v<Iterable>) {
            if (_iterable.size() < _pair_size) {
                return { _iterable, _iterable.end(), _pair_size };
            }
            return { _iterable, _iterable.begin(), _pair_size };
        }
        else if constexpr (is_bidi_tag_v<it_cat> && !has_sentinel_v<Iterable>) {
            std::size_t count = 0;
            auto it = _iterable.end();
            for (; count < _pair_size && it != _iterable.begin(); --it, ++count) {
            }
            if (count < _pair_size) {
                return { _iterable, _iterable.end(), _pair_size };
            }
        }
        else {
            std::size_t count = 0;
            auto it = _iterable.begin();
            for (; count < _pair_size && it != _iterable.end(); ++it, ++count) {
            }
            if (it == _iterable.end() && count < _pair_size) {
                return { _iterable, it, _pair_size };
            }
        }
        return { _iterable, _iterable.begin(), _pair_size };
    }

    [[nodiscard]] constexpr auto end() const {
        if constexpr (return_sentinel) {
            return default_sentinel;
        }
        else if constexpr (is_ra_tag_v<it_cat>) {
            const auto end_offset = std::min(static_cast<diff>(_pair_size) - 1, _iterable.end() - _iterable.begin());
            return iterator{ _iterable, _iterable.end() - end_offset, _pair_size };
        }
        else {
            return iterator{ _iterable, _iterable.end(), _pair_size };
        }
    }

#else

    template<class Cat = it_cat>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<Cat>::value, iterator> begin() const {
        return { _iterable, _iterable.begin(), _pair_size };
    }

    // clang-format off

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<!is_ra_tag<it_cat>::value && is_sized<I>::value && !has_sentinel<I>::value, iterator>
    begin() const {
        if (_iterable.size() < _pair_size) {
            return { _iterable, _iterable.end(), _pair_size };
        }
        return { _iterable, _iterable.begin(), _pair_size };
    }

    template<class Cat = it_cat>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<!is_ra_tag<Cat>::value && !has_sentinel<Iterable>::value && !is_sized<Iterable>::value, iterator>
    begin() const {
        std::size_t count = 0;
        auto it = _iterable.end();
        for (; count < _pair_size && it != _iterable.begin(); --it, ++count) {
        }
        if (count < _pair_size) {
            return { _iterable, _iterable.end(), _pair_size };
        }
        return { _iterable, it, _pair_size };
    }

    // clang-format on

    template<class Cat = it_cat>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<Cat>::value, iterator> begin() const {
        std::size_t count = 0;
        auto it = _iterable.begin();
        for (; count < _pair_size && it != _iterable.end(); ++it, ++count) {
        }
        if (it == _iterable.end() && count < _pair_size) {
            return { _iterable, it, _pair_size };
        }
        return { _iterable, _iterable.begin(), _pair_size };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, default_sentinel_t> end() const {
        return default_sentinel;
    }

    template<class Cat = it_cat>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<Cat>::value && !return_sentinel, iterator> end() const {
        const auto end_offset = std::min(static_cast<diff>(_pair_size) - 1, _iterable.end() - _iterable.begin());
        return iterator{ _iterable, _iterable.end() - end_offset, _pair_size };
    }

    template<class Cat = it_cat>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_ra_tag<Cat>::value && !return_sentinel, iterator> end() const {
        return iterator{ _iterable, _iterable.end(), _pair_size };
    }

#endif
};

} // namespace detail
} // namespace lz

#endif
