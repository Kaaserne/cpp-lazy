#pragma once

#ifndef LZ_C_STRING_HPP
#define LZ_C_STRING_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/c_string.hpp"
#include "Lz/detail/traits.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class C, class Tag>
class c_string_iterable final
    : public detail::basic_iterable<detail::c_string_iterator<C, Tag>, typename detail::c_string_iterator<C, Tag>::sentinel> {
    using s = typename detail::c_string_iterator<C, Tag>::sentinel;

    static_assert(std::is_pointer<C>::value, "C must be a pointer");

public:
    using iterator = detail::c_string_iterator<C, Tag>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr c_string_iterable(C begin) noexcept : detail::basic_iterable<iterator, s>(iterator(begin)) {
    }

    constexpr c_string_iterable(C begin, C end) noexcept :
        detail::basic_iterable<iterator, iterator>(iterator(begin), iterator(end)) {
    }

    constexpr c_string_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Creates a c-string iterator with knowing its length
 *
 * @param begin Beginning of the c string
 * @param end Ending of the c string
 * @return CString object containing a random access iterator
 */
template<class C>
LZ_NODISCARD constexpr c_string_iterable<const C*, std::random_access_iterator_tag>
c_string(const C* begin, const C* end) noexcept {
    return { begin, end };
}

/**
 * Creates a c-string iterator with knowing its length
 *
 * @param arr Beginning array of the c string
 * @return CString object containing a random access iterator
 */
template<class C, std::size_t N, detail::enable_if<!std::is_const<C>::value, int> = 0>
LZ_NODISCARD constexpr c_string_iterable<C*, std::random_access_iterator_tag> c_string(C (&arr)[N]) noexcept {
    return { detail::begin(arr), detail::end(arr) };
}

/**
 * Creates a c-string iterator without knowing its length
 *
 * @param s The c string
 * @return CString object containing a forward iterator
 */
template<class C>
LZ_NODISCARD constexpr c_string_iterable<const C*, std::forward_iterator_tag> c_string(const C* s) noexcept {
    return { s };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif