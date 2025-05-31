#include <c_string_forward_decl.hpp>

template class lz::detail::c_string_iterable<const char>;
template class lz::detail::c_string_iterator<const char>;
template class lz::detail::c_string_iterable<const char> lz::detail::c_string_adaptor::operator()(const char* str) const noexcept;
