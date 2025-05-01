#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#define LZ_GENERATE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class GeneratorFunc, bool /* is inf */>
class generate_iterator;

template<class GeneratorFunc>
class generate_iterator<GeneratorFunc, false>
    : public iterator<generate_iterator<GeneratorFunc, false>, func_ret_type<GeneratorFunc>,
                      fake_ptr_proxy<func_ret_type<GeneratorFunc>>, std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {

    mutable GeneratorFunc _func;
    std::size_t _current{};

public:
    using reference = func_ret_type<GeneratorFunc>;
    using value_type = decay_t<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

    constexpr generate_iterator() = default;

    constexpr generate_iterator(GeneratorFunc generator_func, const std::size_t amount) :
        _func{ std::move(generator_func) },
        _current{ amount } {
    }

    LZ_CONSTEXPR_CXX_14 generate_iterator& operator=(default_sentinel) noexcept {
        _current = 0;
    }

    constexpr reference dereference() const {
        return _func();
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        --_current;
    }

    constexpr bool eq(const generate_iterator& b) const noexcept {
        return _current == b._current;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _current == 0;
    }
};

template<class GeneratorFunc>
class generate_iterator<GeneratorFunc, true>
    : public iterator<generate_iterator<GeneratorFunc, true>, func_ret_type<GeneratorFunc>,
                      fake_ptr_proxy<func_ret_type<GeneratorFunc>>, std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {

    GeneratorFunc _func;

public:
    using reference = func_ret_type<GeneratorFunc>;
    using value_type = decay_t<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

    constexpr generate_iterator() = default;

    constexpr generate_iterator(GeneratorFunc generator_func) : _func{ std::move(generator_func) } {
    }

    constexpr reference dereference() const {
        return _func();
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() const noexcept {
    }

    constexpr bool eq(const generate_iterator&) const noexcept {
        return false;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return false;
    }
};
} // namespace detail
} // namespace lz

#endif