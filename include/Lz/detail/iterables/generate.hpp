#pragma once

#ifndef LZ_GENERATE_ITERABLE_HPP
#define LZ_GENERATE_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/generate.hpp>

namespace lz {
namespace detail {
template<class, bool /* is inf */>
class generate_iterable;

template<class GeneratorFunc>
class generate_iterable<GeneratorFunc, false> : public lazy_view {
    func_container<GeneratorFunc> _func;
    std::size_t _amount{};

public:
    using iterator = generate_iterator<func_container<GeneratorFunc>, false>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr generate_iterable()
        requires std::default_initializable<GeneratorFunc>
    = default;

#else

    template<class G = GeneratorFunc, class = enable_if<std::is_default_constructible<G>::value>>
    constexpr generate_iterable() {
    }

#endif

    constexpr generate_iterable(GeneratorFunc func, const std::size_t amount) : _func{ std::move(func) }, _amount{ amount } {
    }

    LZ_NODISCARD constexpr std::size_t size() const noexcept {
        return _amount;
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return { _func, _amount };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { std::move(_func), _amount };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};

template<class GeneratorFunc>
class generate_iterable<GeneratorFunc, true> : public lazy_view {
    func_container<GeneratorFunc> _func;

public:
    using iterator = generate_iterator<func_container<GeneratorFunc>, true>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr generate_iterable()
        requires std::default_initializable<GeneratorFunc>
    = default;

#else

    template<class G = GeneratorFunc, class = enable_if<std::is_default_constructible<G>::value>>
    constexpr generate_iterable() {
    }

#endif

    constexpr generate_iterable(GeneratorFunc func) : _func{ std::move(func) } {
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return { _func };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { std::move(_func) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif