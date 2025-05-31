#pragma once

#ifndef LZ_RANGE_ITERABLE_HPP
#define LZ_RANGE_ITERABLE_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterators/range.hpp>

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

    LZ_CONSTEXPR_CXX_14 range_iterable(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        _start{ start },
        _end{ end },
        _step{ step } {
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        LZ_ASSERT(_step != 0, "Division by zero in range size calculation");
        if constexpr (std::is_floating_point_v<Arithmetic>) {
            const auto total_length = (_end - _start) / static_cast<Arithmetic>(_step);
            const auto int_part = static_cast<std::size_t>(total_length);
            return (total_length > static_cast<Arithmetic>(int_part)) ? int_part + 1 : int_part;
        }
        else {
            const auto diff = _end - _start;
            return static_cast<std::size_t>(std::abs((diff + (_step > 0 ? _step - 1 : _step + 1)) / _step));
        }
    }

#else

    template<class A = Arithmetic>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<std::is_floating_point<A>::value, std::size_t> size() const noexcept {
        LZ_ASSERT(_step != 0, "Division by zero in range size calculation");
        const auto total_length = (_end - _start) / static_cast<Arithmetic>(_step);
        const auto int_part = static_cast<std::size_t>(total_length);
        return (total_length > static_cast<A>(int_part)) ? int_part + 1 : int_part;
    }

    template<class A = Arithmetic>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!std::is_floating_point<A>::value, std::size_t> size() const noexcept {
        LZ_ASSERT(_step != 0, "Division by zero in range size calculation");
        const auto diff = _end - _start;
        return static_cast<std::size_t>(std::abs((diff + (_step > 0 ? _step - 1 : _step + 1)) / _step));
    }

#endif

    LZ_NODISCARD constexpr iterator begin() const noexcept {
        return { _start, _step };
    }

    LZ_NODISCARD constexpr iterator end() const noexcept {
        return { _start + static_cast<Arithmetic>(size()) * _step, _step };
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_RANGE_ITERABLE_HPP