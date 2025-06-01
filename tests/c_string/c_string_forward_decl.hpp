#pragma once

#ifndef LZ_C_STRING_FORWARD_DECL_HPP
#define LZ_C_STRING_FORWARD_DECL_HPP

#include <Lz/c_string.hpp>

extern template class lz::detail::c_string_iterable<const char>;
extern template class lz::detail::c_string_iterator<const char>;
extern template lz::detail::c_string_iterable<const char>
lz::detail::c_string_adaptor::operator()<const char>(const char* str) const noexcept;

#endif // LZ_C_STRING_FORWARD_DECL_HPP
