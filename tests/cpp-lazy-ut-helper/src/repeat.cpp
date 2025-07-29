#include "cpp-lazy-ut-helper/repeat.hpp"

#include <Lz/detail/adaptors/repeat.hpp>
#include <Lz/detail/iterables/repeat.hpp>
#include <Lz/detail/iterators/repeat.hpp>

template class lz::detail::repeat_iterable<false, int>;
template class lz::detail::repeat_iterator<false, int>;

#if defined(_MSC_VER) && _MSC_VER >= 1920 && _MSC_VER < 1930

// Workaround for msvc 2019 'must return a value' error
template<>
lz::detail::repeat_iterable<false, int> lz::detail::repeat_adaptor::operator()<int>(int value, const std::size_t amount) const {
    return { str };
}

#else

template lz::detail::repeat_iterable<false, int>
lz::detail::repeat_adaptor::operator()<int>(int value, const std::size_t amount) const;

#endif
