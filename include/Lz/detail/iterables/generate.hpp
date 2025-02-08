#pragma once

#ifndef LZ_GENERATE_ITERABLE_HPP
#define LZ_GENERATE_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/generate.hpp>

namespace lz {
namespace detail {
template<class GeneratorFunc>
class generate_iterable : public lazy_view {
    func_container<GeneratorFunc> _func;
    std::size_t _amount;

public:
    using iterator = generate_iterator<func_container<GeneratorFunc>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr generate_iterable() = default;

    constexpr generate_iterable(GeneratorFunc func, const std::size_t amount) : _func{ std::move(func) }, _amount{ amount } {
    }

    LZ_NODISCARD constexpr std::size_t size() const noexcept {
        return _amount;
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return { _func, _amount };
    }

    LZ_NODISCARD constexpr iterator begin() && {
        return { std::move(_func), _amount };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif