#pragma once

#ifndef LZ_GENERATE_WHILE_ITERATOR_HPP
#define LZ_GENERATE_WHILE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {

template<class GeneratorFunc>
class generate_while_iterator
    : public iterator<generate_while_iterator<GeneratorFunc>, tup_element<1, func_ret_type<GeneratorFunc>>,
                      fake_ptr_proxy<tup_element<1, func_ret_type<GeneratorFunc>>>, std::ptrdiff_t, std::forward_iterator_tag,
                      default_sentinel_t> {

    mutable GeneratorFunc _func;
    using fn_return_type = decltype(_func());
    fn_return_type _last_returned{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr generate_while_iterator()
        requires(std::default_initializable<GeneratorFunc> && std::default_initializable<fn_return_type>)
    = default;

#else

    template<class G = GeneratorFunc,
             class = enable_if<std::is_default_constructible<G>::value && std::is_default_constructible<fn_return_type>::value>>
    constexpr generate_while_iterator() noexcept(std::is_nothrow_default_constructible<G>::value &&
                                                 std::is_nothrow_default_constructible<fn_return_type>::value) {
    }

#endif

    using reference = tup_element<1, fn_return_type>;
    using value_type = remove_cvref<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

    LZ_CONSTEXPR_CXX_14 generate_while_iterator(GeneratorFunc generator_func, fn_return_type last_returned) :
        _func{ std::move(generator_func) },
        _last_returned{ std::move(last_returned) } {
    }

    LZ_CONSTEXPR_CXX_14 generate_while_iterator& operator=(default_sentinel_t) noexcept {
        std::get<0>(_last_returned) = false;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return std::get<1>(_last_returned);
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        _last_returned = _func();
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const generate_while_iterator&) const noexcept {
        return eq(lz::default_sentinel);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel_t) const noexcept {
        return !std::get<0>(_last_returned);
    }
};
} // namespace detail
} // namespace lz

#endif
