#include "cpp-lazy-ut-helper/ut_helper.hpp"

#include <Lz/detail/adaptors/c_string.hpp>
#include <Lz/detail/iterables/c_string.hpp>
#include <Lz/detail/iterators/c_string.hpp>

template class lz::detail::c_string_iterable<const char>;
template class lz::detail::c_string_iterator<const char>;

template lz::detail::c_string_iterable<const char>
lz::detail::c_string_adaptor::operator()<const char>(const char* str) const noexcept;

template class std::vector<int>;
template class std::vector<char>;
template class std::vector<std::string>;

template class std::basic_string<char>;

template class std::list<int>;

template class std::forward_list<int>;
