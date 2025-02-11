#pragma once

#ifndef LZ_ZIP_ADAPTOR_HPP
#define LZ_ZIP_ADAPTOR_HPP

#include <Lz/detail/iterables/zip.hpp>

namespace lz {
namespace detail {
struct zip_adaptor {
    using adaptor = zip_adaptor;

    template<class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_iterable<Iterables...> operator()(Iterables&&... iterables) const {
        return { std::forward<Iterables>(iterables)... };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_ADAPTOR_HPP
