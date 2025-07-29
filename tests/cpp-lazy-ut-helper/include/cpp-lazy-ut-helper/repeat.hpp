#pragma once

#ifndef LZ_UT_HELPER_REPEAT
#define LZ_UT_HELPER_REPEAT

#include <Lz/repeat.hpp>

extern template class lz::detail::repeat_iterable<false, int>;
extern template class lz::detail::repeat_iterator<false, int>;

extern template lz::detail::repeat_iterable<false, int> lz::detail::repeat_adaptor::operator()<int>(int, std::size_t) const;

#endif // LZ_UT_HELPER_REPEAT
