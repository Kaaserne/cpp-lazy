#pragma once

#ifndef LZ_RANGE_ITERABLE_HPP
#define LZ_RANGE_ITERABLE_HPP

#include <Lz/detail/iterators/range.hpp>
#include <Lz/detail/concepts.hpp>

template<LZ_CONCEPT_ARITHMETIC Arithmetic>
class range_iterable {
    Arithmetic _start;
    Arithmetic _end;
    Arithmetic _step;

public:
    using iterator = range_iterator<Arithmetic>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using value_type = typename iterator::value_type;

    constexpr range_iterable(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        _start{ start },
        _end{ end },
        _step{ step } {
    }

    LZ_NODISCARD constexpr iterator begin() const noexcept {
        return { _start, _step };
    }

    LZ_NODISCARD constexpr iterator end() const noexcept {
        return { _end, _step };
    }

    constexpr range_iterable() = default;
};

#endif // LZ_RANGE_ITERABLE_HPP