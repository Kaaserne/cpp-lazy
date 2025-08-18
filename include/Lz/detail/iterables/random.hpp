#pragma once

#ifndef LZ_RANDOM_ITERABLE_HPP
#define LZ_RANDOM_ITERABLE_HPP

#include <Lz/detail/iterators/random.hpp>

namespace lz {
namespace detail {
template<class Arithmetic, class Distribution, class Generator, bool UseSentinel>
class random_iterable : public lazy_view {
    Distribution _distribution;
    Generator* _generator{ nullptr };
    size_t _current{};

public:
    using iterator = random_iterator<Arithmetic, Distribution, Generator, UseSentinel>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr random_iterable()
        requires(std::default_initializable<Distribution>)
    = default;

#else

    template<class D = Distribution, class = enable_if<std::is_default_constructible<D>::value>>
    constexpr random_iterable() noexcept(std::is_nothrow_default_constructible<D>::value) {
    }

#endif

    constexpr random_iterable(const Distribution& distribution, Generator& generator, const size_t current) :
        _distribution{ distribution },
        _generator{ &generator },
        _current{ current } {
    }

    LZ_NODISCARD constexpr size_t size() const {
        return static_cast<size_t>(_current);
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return { _distribution, *_generator, _current };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!UseSentinel) {
            return iterator{ _distribution, *_generator, 0 };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool B = UseSentinel>
    LZ_NODISCARD constexpr enable_if<!B, iterator> end() const {
        return { _distribution, *_generator, 0 };
    }

    template<bool B = UseSentinel>
    LZ_NODISCARD constexpr enable_if<B, default_sentinel_t> end() const {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_RANDOM_ITERABLE_HPP
