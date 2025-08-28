#pragma once

#ifndef LZ_UT_HELPER_LIB_C_STRING_HPP
#define LZ_UT_HELPER_LIB_C_STRING_HPP

#include <Lz/c_string.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>

extern template class lz::detail::c_string_iterable<const char>;
extern template class lz::detail::c_string_iterator<const char>;

extern template lz::detail::c_string_iterable<const char>
lz::detail::c_string_adaptor::operator()<const char>(const char*) const noexcept;

extern template class std::vector<int>;
extern template class std::vector<char>;
extern template class std::vector<std::string>;

extern template class std::basic_string<char>;

extern template class std::list<int>;

extern template class std::forward_list<int>;

#endif // LZ_UT_HELPER_LIB_C_STRING_HPP
