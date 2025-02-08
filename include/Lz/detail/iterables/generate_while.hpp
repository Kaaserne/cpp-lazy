#pragma once

#ifndef LZ_GENERATE_WHILE_ITERABLE_HPP
#define LZ_GENERATE_WHILE_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/generate_while.hpp>

namespace lz {
namespace detail {
template<class GeneratorFunc>
class generate_while_iterable : public lazy_view {
    func_container<GeneratorFunc> _func;
    decltype(_func()) _init;

public:
    using iterator = generate_while_iterator<func_container<GeneratorFunc>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr generate_while_iterable() = default;

    LZ_CONSTEXPR_CXX_14 generate_while_iterable(GeneratorFunc func) : _func{ std::move(func) }, _init{ _func() } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _func, _init };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { std::move(_func), std::move(_init) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif