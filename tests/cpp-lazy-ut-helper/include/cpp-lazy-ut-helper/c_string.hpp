#pragma once

#ifndef LZ_UT_HELPER_LIB_C_STRING_HPP
#define LZ_UT_HELPER_LIB_C_STRING_HPP

#include <Lz/c_string.hpp>

extern template class lz::detail::c_string_iterable<const char>;
extern template class lz::detail::c_string_iterator<const char>;

extern template lz::detail::c_string_iterable<const char>
lz::detail::c_string_adaptor::operator()<const char>(const char*) const noexcept;

#endif // LZ_UT_HELPER_LIB_C_STRING_HPP
