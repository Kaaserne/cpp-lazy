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
    std::ptrdiff_t _current;

public:
    using iterator = random_iterator<Arithmetic, Distribution, Generator, UseSentinel>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr random_iterable(const Distribution& distribution, Generator& generator, const std::ptrdiff_t current) :
        _distribution{ distribution },
        _generator{ &generator },
        _current{ current } {
    }

    LZ_NODISCARD constexpr std::size_t size() const {
        return static_cast<std::size_t>(_current);
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return { _distribution, *_generator, _current };
    }

    template<bool B = UseSentinel>
    LZ_NODISCARD constexpr enable_if<!B, iterator> end() const {
        return { _distribution, *_generator, 0 };
    }

    template<bool B = UseSentinel>
    LZ_NODISCARD constexpr enable_if<B, default_sentinel> end() const {
        return {};
    }
};
} // namespace detail
} // namespace lz


#endif // LZ_RANDOM_ITERABLE_HPP