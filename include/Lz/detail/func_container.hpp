#pragma once

#ifndef LZ_FUNCTION_CONTAINER_HPP
#define LZ_FUNCTION_CONTAINER_HPP

#include <Lz/detail/procs.hpp>
#include <type_traits>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class>
constexpr bool is_reference_wrapper_v = false;

template<class U>
constexpr bool is_reference_wrapper_v<std::reference_wrapper<U>> = true;

template<class C, class Pointed, class Object, class... Args>
constexpr decltype(auto) invoke(Pointed C::*member, Object&& object, Args&&... args) {
    using object_t = remove_cvref<Object>;
    constexpr bool is_wrapped = is_reference_wrapper_v<object_t>;
    constexpr bool is_derived_object = std::is_same_v<C, object_t> || std::is_base_of_v<C, object_t>;

    if constexpr (std::is_function_v<Pointed>) {
        if constexpr (is_derived_object) {
            return (std::forward<Object>(object).*member)(std::forward<Args>(args)...);
        }
        else if constexpr (is_wrapped) {
            return (object.get().*member)(std::forward<Args>(args)...);
        }
        else {
            return ((*std::forward<Object>(object)).*member)(std::forward<Args>(args)...);
        }
    }
    else {
        static_assert(std::is_object_v<Pointed> && sizeof...(args) == 0);
        if constexpr (is_derived_object) {
            return std::forward<Object>(object).*member;
        }
        else if constexpr (is_wrapped) {
            return object.get().*member;
        }
        else {
            return (*std::forward<Object>(object)).*member;
        }
    }
}

#else
// TODO write tests
template<class T>
struct is_reference_wrapper : std::false_type {};

template<class T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<std::is_function<Pointed>::value && (std::is_same<C, remove_cvref<Object>>::value ||
                                                                           std::is_base_of<C, remove_cvref<Object>>::value),
                                      decltype((std::forward<Object>(object).*member)(std::forward<Args>(args)...))> {

    return (std::forward<Object>(object).*member)(std::forward<Args>(args)...);
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<std::is_function<Pointed>::value && is_reference_wrapper<remove_cvref<Object>>::value,
                                      decltype((object.get().*member)(std::forward<Args>(args)...))> {

    return (std::forward<Object>(object).*member)(std::forward<Args>(args)...);
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto invoke(Pointed C::*member, Object&& object, Args&&... args)
    -> enable_if_t<std::is_function<Pointed>::value && !is_reference_wrapper<remove_cvref<Object>>::value &&
                       !(std::is_same<C, remove_cvref<Object>>::value || std::is_base_of<C, remove_cvref<Object>>::value),
                   decltype(((*std::forward<Object>(object)).*member)(std::forward<Args>(args)...))> {

    return ((*std::forward<Object>(object)).*member)(std::forward<Args>(args)...);
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<!std::is_function<Pointed>::value && (std::is_same<C, remove_cvref<Object>>::value ||
                                                                            std::is_base_of<C, remove_cvref<Object>>::value),
                                      decltype(std::forward<Object>(object).*member)> {

    static_assert(std::is_object<Pointed>::value && sizeof...(args) == 0, "Member pointer must point to an object type");
    return std::forward<Object>(object).*member;
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto
invoke(Pointed C::*member, Object&& object,
       Args&&... args) -> enable_if_t<!std::is_function<Pointed>::value && is_reference_wrapper<remove_cvref<Object>>::value,
                                      decltype(object.get().*member)> {

    static_assert(std::is_object<Pointed>::value && sizeof...(args) == 0, "Member pointer must point to an object type");
    return object.get().*member;
}

template<class C, class Pointed, class Object, class... Args>
constexpr auto invoke(Pointed C::*member, Object&& object, Args&&... args)
    -> enable_if_t<!std::is_function<Pointed>::value && !is_reference_wrapper<remove_cvref<Object>>::value &&
                       !(std::is_same<C, remove_cvref<Object>>::value || std::is_base_of<C, remove_cvref<Object>>::value),
                   decltype((*std::forward<Object>(object)).*member)> {

    static_assert(std::is_object<Pointed>::value && sizeof...(args) == 0, "Member pointer must point to an object type");
    return (*std::forward<Object>(object)).*member;
}

#endif

template<class Func>
class func_container {
    Func _func;

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
        // return invoke(_func, std::forward<Args>(args)...);
        if constexpr (std::is_member_pointer_v<Func>) {
            return invoke(_func, std::forward<Args>(args)...);
        }
        else {
            return _func(std::forward<Args>(args)...);
        }
    }

    template<class... Args>
    [[nodiscard]] constexpr decltype(auto) operator()(Args&&... args) & {
        if constexpr (std::is_member_pointer_v<Func>) {
            return invoke(_func, std::forward<Args>(args)...);
        }
        else {
            return _func(std::forward<Args>(args)...);
        }
    }

    template<class... Args>
    [[nodiscard]] constexpr decltype(auto) operator()(Args&&... args) && {
        if constexpr (std::is_member_pointer_v<Func>) {
            return invoke(_func, std::forward<Args>(args)...);
        }
        else {
            return std::move(_func)(std::forward<Args>(args)...);
        }
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
