#pragma once

#ifndef LZ_FUNCTION_CONTAINER_HPP
#define LZ_FUNCTION_CONTAINER_HPP

#include <Lz/detail/traits.hpp>
#include <type_traits>

#ifdef LZ_HAS_CXX_17

#include <functional> // std::invoke

#endif

namespace lz {
namespace detail {

#ifndef LZ_HAS_CXX_17

template<class Fn, class... Args>
constexpr auto invoke(Fn&& fn, Args&&... args)
    -> enable_if<!std::is_member_pointer<decay_t<Fn>>::value, decltype(std::forward<Fn>(fn)(std::forward<Args>(args)...))> {
    return std::forward<Fn>(fn)(std::forward<Args>(args)...);
}

template<class MemFn, class T, class... Args>
constexpr auto
invoke(MemFn T::*fn, T&& obj, Args&&... args) -> enable_if<std::is_member_function_pointer<MemFn T::*>::value,
                                                           decltype((std::forward<T>(obj).*fn)(std::forward<Args>(args)...))> {
    return (std::forward<T>(obj).*fn)(std::forward<Args>(args)...);
}

template<class MemFn, class T, class... Args>
constexpr auto invoke(MemFn T::*fn, T* obj, Args&&... args)
    -> enable_if<std::is_member_function_pointer<MemFn T::*>::value, decltype((obj->*fn)(std::forward<Args>(args)...))> {
    return (obj->*fn)(std::forward<Args>(args)...);
}

template<class MemData, class T>
constexpr auto
invoke(MemData T::*md,
       T&& obj) -> enable_if<std::is_member_object_pointer<MemData T::*>::value, decltype(std::forward<T>(obj).*md)> {
    return std::forward<T>(obj).*md;
}

template<class MemData, class T>
constexpr auto
invoke(MemData T::*md, T* obj) -> enable_if<std::is_member_object_pointer<MemData T::*>::value, decltype(obj->*md)> {
    return obj->*md;
}

#endif

#ifdef LZ_HAS_CXX_17

using std::invoke;

#endif

template<class Func>
class func_container {
    Func _func;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr func_container()
        requires std::default_initializable<Func>
    = default;

#else

    template<class F = Func, class = enable_if<std::is_default_constructible<F>::value>>
    constexpr func_container() noexcept(std::is_nothrow_default_constructible<Func>::value) {
    }

#endif

    explicit func_container(const Func& func) : _func{ func } {
    }

    explicit func_container(Func&& func) noexcept(std::is_nothrow_copy_constructible<Func>::value) : _func{ std::move(func) } {
    }

    func_container(const func_container& other) : _func{ other._func } {
    }

    func_container(func_container&& other) noexcept(std::is_nothrow_copy_constructible<Func>::value) :
        _func(std::move(other._func)) {
    }

    func_container& operator=(const func_container& other) {
        ::new (static_cast<void*>(std::addressof(_func))) Func(other._func);
        return *this;
    }

    func_container& operator=(func_container&& other) noexcept(std::is_nothrow_move_assignable<Func>::value) {
        _func.~Func();
        ::new (static_cast<void*>(std::addressof(_func))) Func(static_cast<Func&&>(other._func));
        return *this;
    }

    template<class... Args>
    constexpr auto operator()(Args&&... args) const& -> decltype(invoke(_func, std::forward<Args>(args)...)) {
        return invoke(_func, std::forward<Args>(args)...);
    }

    template<class... Args>
    LZ_CONSTEXPR_CXX_14 auto operator()(Args&&... args) & -> decltype(invoke(_func, std::forward<Args>(args)...)) {
        return invoke(_func, std::forward<Args>(args)...);
    }

    template<class... Args>
    LZ_CONSTEXPR_CXX_14 auto operator()(Args&&... args) && -> decltype(invoke(std::move(_func), std::forward<Args>(args)...)) {
        return invoke(std::move(_func), std::forward<Args>(args)...);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_FUNCTION_CONTAINER_HPP