#pragma once

#ifndef LZ_PAIRWISE_ITERABLE_HPP
#define LZ_PAIRWISE_ITERABLE_HPP

#include <Lz/detail/iterators/pairwise.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class Iterable>
class pairwise_iterable : public lazy_view {
    static constexpr auto is_bidi = is_bidi_tag<iter_cat_iterable_t<Iterable>>::value;
    static constexpr auto is_rand_it = is_ra_tag<iter_cat_iterable_t<Iterable>>::value;
    static constexpr auto is_sentinel = has_sentinel<Iterable>::value;
    static constexpr auto return_sentinel = !is_bidi || has_sentinel<Iterable>::value;

    using iterable = basic_iterable<iter_t<Iterable>>;

    maybe_owned<Iterable> _iterable;
    std::size_t _pair_size{};

    LZ_CONSTEXPR_CXX_14 iterable make_corrected_ra_iterable() const {
        const auto current_size = _iterable.end() - _iterable.begin();
        auto end = _iterable.begin() + current_size;
        const auto s_pair_size = static_cast<diff>(_pair_size);

        if (s_pair_size > current_size) {
            return { end, end };
        }

        const auto corrected_length = s_pair_size - 1;
        return { _iterable.begin(), end - corrected_length };
    }

public:
    using iterator = pairwise_iterator<iterable>;
    using value_type = typename iterator::value_type;
    using const_iterator = iterator;

private:
    using diff = typename iterator::difference_type;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr pairwise_iterable()
        requires(std::default_initializable<iterable>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr pairwise_iterable() noexcept(std::is_nothrow_default_constructible<iterable>::value) {
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

    template<bool RA = is_rand_it>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<RA, iterator> begin() const {
        auto corrected_iterable = make_corrected_ra_iterable();
        return { corrected_iterable, corrected_iterable.begin(), _pair_size };
    }

    template<bool RA = is_rand_it>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!RA && is_sized<Iterable>::value && !is_sentinel, iterator> begin() const {
        if (size() == 0) {
            auto end = _iterable.end();
            return { iterable{ end, end }, end, _pair_size };
        }
        iterable new_iterable{ _iterable.begin(), next_fast_safe(_iterable, static_cast<diff>(size())) };
        return { new_iterable, _iterable.begin(), _pair_size };
    }

    template<bool RA = is_rand_it>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!return_sentinel && RA, iterator> end() const {
        auto corrected_iterable = make_corrected_ra_iterable();
        return { corrected_iterable, corrected_iterable.end(), _pair_size };
    }

    template<bool RA = is_rand_it>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!return_sentinel && !RA, iterator> end() const {
        if (static_cast<diff>(_pair_size) > lz::size(_iterable)) {
            auto end = _iterable.end();
            return { iterable{ end, end }, end, _pair_size };
        }

        auto fake_end = next_fast_safe(_iterable, static_cast<diff>(size()));
        iterable new_iterable{ _iterable.begin(), fake_end };
        return { new_iterable, fake_end, _pair_size };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if<R, default_sentinel_t> end() const {
        return default_sentinel;
    }
};

} // namespace detail
} // namespace lz

#endif
