#pragma once

#ifndef LZ_FUNCTION_CONTAINER_HPP
#define LZ_FUNCTION_CONTAINER_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/procs/addressof.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/remove_ref.hpp>
#include <type_traits>

#ifdef LZ_HAS_CXX_17
#include <functional> // for std::invoke
#else
#include <Lz/detail/traits/is_reference_wrapper.hpp>
#endif

namespace lz {
namespace detail {

#ifndef LZ_HAS_CXX_17

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<std::is_function<Pointed>::value && (std::is_same<C, remove_cvref_t<Object>>::value ||
                                                                           std::is_base_of<C, remove_cvref_t<Object>>::value),
                                      decltype((std::forward<Object>(object).*member)(std::forward<Args>(args)...))> {

    return (std::forward<Object>(object).*member)(std::forward<Args>(args)...);
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<std::is_function<Pointed>::value && is_reference_wrapper<remove_cvref_t<Object>>::value,
                                      decltype((object.get().*member)(std::forward<Args>(args)...))> {

    return (std::forward<Object>(object).*member)(std::forward<Args>(args)...);
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto invoke(Pointed C::*member, Object&& object, Args&&... args)
    -> enable_if_t<std::is_function<Pointed>::value && !is_reference_wrapper<remove_cvref_t<Object>>::value &&
                       !(std::is_same<C, remove_cvref_t<Object>>::value || std::is_base_of<C, remove_cvref_t<Object>>::value),
                   decltype(((*std::forward<Object>(object)).*member)(std::forward<Args>(args)...))> {

    return ((*std::forward<Object>(object)).*member)(std::forward<Args>(args)...);
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<!std::is_function<Pointed>::value && (std::is_same<C, remove_cvref_t<Object>>::value ||
                                                                            std::is_base_of<C, remove_cvref_t<Object>>::value),
                                      decltype(std::forward<Object>(object).*member)> {

    static_assert(std::is_object<Pointed>::value && sizeof...(args) == 0, "Member pointer must point to an object type");
    return std::forward<Object>(object).*member;
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<!std::is_function<Pointed>::value && is_reference_wrapper<remove_cvref_t<Object>>::value,
                                      decltype(object.get().*member)> {

    static_assert(std::is_object<Pointed>::value && sizeof...(args) == 0, "Member pointer must point to an object type");
    return object.get().*member;
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto invoke(Pointed C::*member, Object&& object, Args&&... args)
    -> enable_if_t<!std::is_function<Pointed>::value && !is_reference_wrapper<remove_cvref_t<Object>>::value &&
                       !(std::is_same<C, remove_cvref_t<Object>>::value || std::is_base_of<C, remove_cvref_t<Object>>::value),
                   decltype((*std::forward<Object>(object)).*member)> {

    static_assert(std::is_object<Pointed>::value && sizeof...(args) == 0, "Member pointer must point to an object type");
    return (*std::forward<Object>(object)).*member;
}

#endif

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

#ifdef LZ_HAS_CXX_17

    template<class... Args>
    [[nodiscard]] constexpr decltype(auto) operator()(Args&&... args) const& {
        return std::invoke(_func, std::forward<Args>(args)...);
    }

    template<class... Args>
    [[nodiscard]] constexpr decltype(auto) operator()(Args&&... args) & {
        return std::invoke(_func, std::forward<Args>(args)...);
    }

    template<class... Args>
    [[nodiscard]] constexpr decltype(auto) operator()(Args&&... args) && {
        return std::invoke(std::move(_func), std::forward<Args>(args)...);
    }

#else

    template<class F = Func, class... Args>
    LZ_NODISCARD constexpr auto operator()(Args&&... args)
        const& -> enable_if_t<std::is_member_pointer<F>::value, decltype(invoke(_func, std::forward<Args>(args)...))> {
        return invoke(_func, std::forward<Args>(args)...);
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD constexpr auto operator()(
        Args&&... args) const& -> enable_if_t<!std::is_member_pointer<F>::value, decltype(_func(std::forward<Args>(args)...))> {
        return _func(std::forward<Args>(args)...);
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto operator()(
        Args&&... args) & -> enable_if_t<std::is_member_pointer<F>::value, decltype(invoke(_func, std::forward<Args>(args)...))> {
        return invoke(_func, std::forward<Args>(args)...);
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto
    operator()(Args&&... args) & -> enable_if_t<!std::is_member_pointer<F>::value, decltype(_func(std::forward<Args>(args)...))> {
        return _func(std::forward<Args>(args)...);
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto
    operator()(Args&&... args) && -> enable_if_t<std::is_member_pointer<F>::value,
                                                 decltype(invoke(_func, std::forward<Args>(args)...))> {
        return invoke(_func, std::forward<Args>(args)...);
    }

    template<class F = Func, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto
    operator()(Args&&... args) && -> enable_if_t<!std::is_member_pointer<F>::value,
                                                 decltype(std::move(_func)(std::forward<Args>(args)...))> {
        return std::move(_func)(std::forward<Args>(args)...);
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_FUNCTION_CONTAINER_HPP
