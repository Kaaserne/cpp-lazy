#pragma once

#ifndef LZ_DETAIL_COMMON_ITERATOR_HPP
#define LZ_DETAIL_COMMON_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/variant.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterator, class S>
class common_iterator : public iter_base<common_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                                         diff_type<Iterator>, iter_cat_t<Iterator>> {
    variant<Iterator, S> _data{};

    using traits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename traits::iterator_category;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr common_iterator() = default;

    // TODO change () to  {}
    constexpr common_iterator(const Iterator& iter) : _data{ iter } {
    }

    constexpr common_iterator(Iterator&& iter) noexcept : _data{ std::move(iter) } {
    }

    constexpr common_iterator(const S& sent) : _data{ sent } {
    }

    constexpr common_iterator(S&& sent) noexcept : _data{ std::move(sent) } {
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(const Iterator& iter) {
        _data = iter;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(const S& sent) {
        _data = sent;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(Iterator&& iter) noexcept {
        _data = std::move(iter);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 common_iterator& operator=(S&& sent) noexcept {
        _data = std::move(sent);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        return **iter;
    }

    LZ_CONSTEXPR_CXX_17 fake_ptr_proxy<reference> arrow() const {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        return fake_ptr_proxy<decltype(**this)>(**iter);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        ++(*iter);
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        --*iter;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const common_iterator& rhs) const {
#ifdef __cpp_lib_variant
        using std::get;
        using std::get_if;
#endif

        auto&& lhs_iter = get_if<Iterator>(&_data);
        auto&& rhs_iter = get_if<S>(&rhs._data);

        if (lhs_iter && rhs_iter) {
            return *lhs_iter == *rhs_iter;
        }

        auto&& lhs_iter_inverted = get_if<S>(&_data);
        auto&& rhs_iter_inverted = get_if<Iterator>(&rhs._data);

        if (lhs_iter_inverted && rhs_iter_inverted) {
            return *lhs_iter == *rhs_iter;
        }

        return true;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        *iter += n;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const common_iterator& rhs) const {
#ifdef __cpp_lib_variant
        using std::get;
        using std::get_if;
#endif

        auto&& lhs_iter = get_if<Iterator>(&_data);
        auto&& rhs_iter = get_if<Iterator>(&rhs._data);
        return *lhs_iter - *rhs_iter;
    }
};
} // namespace detail
} // namespace lz

#endif