#include "c_string_forward_decl.hpp"

#include <Lz/detail/adaptors/c_string.hpp>
#include <Lz/detail/iterables/c_string.hpp>
#include <Lz/detail/iterators/c_string.hpp>

template class lz::detail::c_string_iterable<const char>;
template class lz::detail::c_string_iterator<const char>;

#if defined(_MSC_VER) && _MSC_VER >= 1920 && _MSC_VER < 1930

// Workaround for msvc 2019 'must return a value' error
template<>
lz::detail::c_string_iterable<const char> lz::detail::c_string_adaptor::operator()<const char>(const char* str) const noexcept {
    return { str };
}

#else

template lz::detail::c_string_iterable<const char>
lz::detail::c_string_adaptor::operator()<const char>(const char* str) const noexcept;

#endif
