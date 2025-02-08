#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#define LZ_GENERATE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class GeneratorFunc>
class generate_iterator : public iter_base<generate_iterator<GeneratorFunc>, func_ret_type<GeneratorFunc>,
                                           fake_ptr_proxy<func_ret_type<GeneratorFunc>>, std::ptrdiff_t,
                                           std::forward_iterator_tag, default_sentinel> {

    mutable GeneratorFunc _func{};
    std::size_t _current{};
    bool _is_inf_loop{};

public:
    using reference = func_ret_type<GeneratorFunc>;
    using value_type = decay_t<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

    constexpr generate_iterator() = default;

    // TODO make generate inf implentation
    constexpr generate_iterator(GeneratorFunc generator_func, const std::size_t amount) :
        _func{ std::move(generator_func) },
        _current{ amount },
        _is_inf_loop{ amount == (std::numeric_limits<std::size_t>::max)() } {
    }

    constexpr reference dereference() const {
        return _func();
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        if (!_is_inf_loop) {
            --_current;
        }
    }

    constexpr bool eq(const generate_iterator& b) const noexcept {
        return _current == b._current;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _current == 0;
    }
};
} // namespace detail
} // namespace lz

#endif