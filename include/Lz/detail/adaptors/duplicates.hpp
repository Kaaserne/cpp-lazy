#ifndef LZ_DUPLICATES_ADAPTOR_HPP
#define LZ_DUPLICATES_ADAPTOR_HPP

#include "Lz/detail/traits.hpp"

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/duplicates.hpp>

// TODO docs

namespace lz {
namespace detail {
struct duplicates_adaptor {
    using adaptor = duplicates_adaptor;

    template<class Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr enable_if<is_iterable<Iterable>::value, duplicates_iterable<remove_ref<Iterable>, BinaryPredicate>>
    operator()(Iterable&& iterable, BinaryPredicate compare = {}) const {
        return { std::forward<Iterable>(iterable), std::move(compare) };
    }

    template<class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, BinaryPredicate>>
    operator()(BinaryPredicate compare = {}) const {
        return { std::move(compare) };
    }
};
} // namespace detail
} // namespace lz

#endif
