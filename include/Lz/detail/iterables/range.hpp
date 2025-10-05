#pragma once

#ifndef LZ_RANGE_ITERABLE_HPP
#define LZ_RANGE_ITERABLE_HPP

#include <Lz/detail/iterators/range.hpp>
#include <Lz/traits/lazy_view.hpp>

namespace lz {
namespace detail {
template<class Arithmetic, bool /* step wise */>
class range_iterable;

template<class Arithmetic>
class range_iterable<Arithmetic, true /* step wise */> : public lazy_view {
    Arithmetic _start{};
    Arithmetic _end{};
    Arithmetic _step{};

public:
    using iterator = range_iterator<Arithmetic, true>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using value_type = typename iterator::value_type;

    constexpr range_iterable() noexcept = default;

#if defined(LZ_USE_DEBUG_ASSERTIONS) && defined(LZ_HAS_CXX_17)

    LZ_CONSTEXPR_CXX_14 range_iterable(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        _start{ start },
        _end{ end },
        _step{ step } {
        if constexpr (!std::is_floating_point_v<Arithmetic>) {
            LZ_ASSERT(step > std::numeric_limits<Arithmetic>::min(), "Step must be larger than the minimum representable value");
            // The behavior (of std::abs) is undefined if the result cannot be represented by the return type
            LZ_ASSERT(std::abs(_step) > 0, "Step cannot be zero");
        }
    }

#elif defined(LZ_USE_DEBUG_ASSERTIONS)

    template<class A = Arithmetic, enable_if_t<!std::is_floating_point<A>::value, int> = 0>
    LZ_CONSTEXPR_CXX_14 range_iterable(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        _start{ start },
        _end{ end },
        _step{ step } {
        LZ_ASSERT(step > std::numeric_limits<Arithmetic>::min(), "Step must be larger than the minimum representable value");
        // The behavior (of std::abs) is undefined if the result cannot be represented by the return type
        LZ_ASSERT(std::abs(_step) > 0, "Step cannot be zero");
    }

    template<class A = Arithmetic, enable_if_t<std::is_floating_point<A>::value, int> = 0>
    LZ_CONSTEXPR_CXX_14 range_iterable(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        _start{ start },
        _end{ end },
        _step{ step } {
    }

#else

    LZ_CONSTEXPR_CXX_14 range_iterable(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        _start{ start },
        _end{ end },
        _step{ step } {
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr size_t size() const noexcept {
        if constexpr (std::is_floating_point_v<Arithmetic>) {
            const auto total_length = (_end - _start) / static_cast<Arithmetic>(_step);
            const auto int_part = static_cast<size_t>(total_length);
            return (total_length > static_cast<Arithmetic>(int_part)) ? int_part + 1 : int_part;
        }
        else {
            const auto diff = _end - _start;
            return static_cast<size_t>(std::abs((diff + (_step > 0 ? _step - 1 : _step + 1)) / _step));
        }
    }

#else

    template<class A = Arithmetic>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<std::is_floating_point<A>::value, size_t> size() const noexcept {
        const auto total_length = (_end - _start) / static_cast<Arithmetic>(_step);
        const auto int_part = static_cast<size_t>(total_length);
        return (total_length > static_cast<A>(int_part)) ? int_part + 1 : int_part;
    }

    template<class A = Arithmetic>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!std::is_floating_point<A>::value, size_t> size() const noexcept {
        const auto diff = _end - _start;
        return static_cast<size_t>(std::abs((diff + (_step > 0 ? _step - 1 : _step + 1)) / _step));
    }

#endif

    LZ_NODISCARD constexpr iterator begin() const noexcept {
        return { _start, _step };
    }

    LZ_NODISCARD constexpr iterator end() const noexcept {
        return { _start + static_cast<Arithmetic>(size()) * _step, _step };
    }
};

template<class Arithmetic>
class range_iterable<Arithmetic, false /* step wise */> : public lazy_view {
    Arithmetic _start{};
    Arithmetic _end{};

public:
    using iterator = range_iterator<Arithmetic, false>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using value_type = typename iterator::value_type;

    constexpr range_iterable() noexcept = default;

    LZ_CONSTEXPR_CXX_14 range_iterable(const Arithmetic start, const Arithmetic end) noexcept : _start{ start }, _end{ end } {
        LZ_ASSERT(start <= end, "Start must be less than or equal to end");
    }

#ifdef LZ_HAS_CXX_17

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 size_t size() const noexcept {
        if constexpr (std::is_floating_point_v<Arithmetic>) {
            const auto total_length = (_end - _start);
            const auto int_part = static_cast<size_t>(total_length);
            return (total_length > static_cast<Arithmetic>(int_part)) ? int_part + 1 : int_part;
        }
        else {
            return static_cast<size_t>(_end - _start);
        }
    }

#else

    template<class A = Arithmetic>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<std::is_floating_point<A>::value, size_t> size() const noexcept {
        const auto total_length = (_end - _start);
        const auto int_part = static_cast<size_t>(total_length);
        return (total_length > static_cast<Arithmetic>(int_part)) ? int_part + 1 : int_part;
    }

    template<class A = Arithmetic>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!std::is_floating_point<A>::value, size_t> size() const noexcept {
        return static_cast<size_t>(_end - _start);
    }

#endif

    LZ_NODISCARD constexpr iterator begin() const noexcept {
        return iterator{ _start };
    }

    LZ_NODISCARD constexpr iterator end() const noexcept {
        return iterator{ _start + static_cast<Arithmetic>(size()) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_RANGE_ITERABLE_HPP
