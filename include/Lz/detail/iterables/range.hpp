#pragma once

#ifndef LZ_RANGE_ITERABLE_HPP
#define LZ_RANGE_ITERABLE_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterators/range.hpp>
#include <cmath>

namespace lz {
namespace detail {
template<class Arithmetic>
class range_iterable : public lazy_view {
    Arithmetic _start;
    Arithmetic _end;
    Arithmetic _step;

public:
    using iterator = range_iterator<Arithmetic>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using value_type = typename iterator::value_type;

    constexpr range_iterable() = default;

    constexpr range_iterable(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        _start{ start },
        _end{ end },
        _step{ step } {
    }

    LZ_NODISCARD std::size_t size() const noexcept {
        if (_step == 0) {
            return 0;
        }
        return static_cast<std::size_t>(std::ceil((_end - _start) / _step));
    }

    LZ_NODISCARD constexpr iterator begin() const noexcept {
        return { _start, _step };
    }

    LZ_NODISCARD constexpr iterator end() const noexcept {
        return { _end, _step };
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_RANGE_ITERABLE_HPP