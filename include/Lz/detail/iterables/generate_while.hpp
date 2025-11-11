#pragma once

#ifndef LZ_GENERATE_WHILE_ITERABLE_HPP
#define LZ_GENERATE_WHILE_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/generate_while.hpp>
#include <Lz/traits/lazy_view.hpp>

namespace lz {
namespace detail {
template<class GeneratorFunc>
class generate_while_iterable : public lazy_view {
    using fn_return_type = decltype(std::declval<GeneratorFunc>()());
    using T = typename std::tuple_element<1, fn_return_type>::type;

    fn_return_type _init{ T{}, true };
    func_container<GeneratorFunc> _func{};

public:
    using iterator = generate_while_iterator<func_container<GeneratorFunc>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr generate_while_iterable()
        requires(std::default_initializable<GeneratorFunc> && std::default_initializable<fn_return_type>)
    = default;

#else

    template<class G = GeneratorFunc,
             class = enable_if_t<std::is_default_constructible<G>::value && std::is_default_constructible<fn_return_type>::value>>
    constexpr generate_while_iterable() noexcept(std::is_nothrow_default_constructible<G>::value &&
                                                 std::is_nothrow_default_constructible<fn_return_type>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 generate_while_iterable(GeneratorFunc func) : _init{ func() }, _func{ std::move(func) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { _func, _init };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 default_sentinel_t end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif
