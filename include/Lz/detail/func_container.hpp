#pragma once

#ifndef LZ_FUNCTION_CONTAINER_HPP
#define LZ_FUNCTION_CONTAINER_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/procs/addressof.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <type_traits>

namespace lz {
namespace detail {

template<class Func>
class func_container {
    Func _func{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr func_container()
        requires(std::default_initializable<Func>)
    = default;

#else

    template<class F = Func, class = enable_if_t<std::is_default_constructible<F>::value>>
    constexpr func_container() noexcept(std::is_nothrow_default_constructible<Func>::value) {
    }

#endif

    explicit func_container(const Func& func) : _func{ func } {
    }

    explicit func_container(Func&& func) noexcept(std::is_nothrow_move_constructible<Func>::value) : _func{ std::move(func) } {
    }

    func_container(const func_container& other) : _func{ other._func } {
    }

    func_container(func_container&& other) noexcept(std::is_nothrow_copy_constructible<Func>::value) :
        _func(std::move(other._func)) {
    }

    func_container& operator=(const func_container& other) {
        ::new (static_cast<void*>(detail::addressof(_func))) Func(other._func);
        return *this;
    }

    func_container& operator=(func_container&& other) noexcept(std::is_nothrow_move_assignable<Func>::value) {
        _func.~Func();
        ::new (static_cast<void*>(detail::addressof(_func))) Func(static_cast<Func&&>(other._func));
        return *this;
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD constexpr auto operator()(Args&&... args) const& -> decltype(_func(std::forward<Args>(args)...)) {
        return _func(std::forward<Args>(args)...);
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto operator()(Args&&... args) & -> decltype(_func(std::forward<Args>(args)...)) {
        return _func(std::forward<Args>(args)...);
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto
    operator()(Args&&... args) && -> decltype(std::move(_func)(std::forward<Args>(args)...)) {
        return std::move(_func)(std::forward<Args>(args)...);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_FUNCTION_CONTAINER_HPP
