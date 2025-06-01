#include "c_string_forward_decl.hpp"

#include <Lz/detail/adaptors/c_string.hpp>
#include <Lz/detail/iterables/c_string.hpp>
#include <Lz/detail/iterators/c_string.hpp>

template class lz::detail::c_string_iterable<const char>;
template class lz::detail::c_string_iterator<const char>;
template class lz::detail::c_string_iterable<const char>
lz::detail::c_string_adaptor::operator()<const char>(const char* str) const noexcept;
