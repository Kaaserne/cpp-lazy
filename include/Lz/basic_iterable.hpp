#pragma once

#ifndef LZ_BASIC_ITERABLE_HPP
#define LZ_BASIC_ITERABLE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/traits/lazy_view.hpp>
#include <Lz/procs/size.hpp>

namespace lz {
namespace detail {
template<class I, class S = I>
class basic_iterable_impl : public lazy_view {
    // we decay I to decay it to a pointer, if it is an array
    using iterator_type = typename std::decay<I>::type;
    using sentinel_type = typename std::decay<S>::type;

    iterator_type _begin{};
    sentinel_type _end{};

    using traits = std::iterator_traits<iterator_type>;

public:
    using iterator = iterator_type;
    using const_iterator = iterator_type;
    using value_type = val_t<iterator_type>;
    using sentinel = sentinel_type;

    constexpr basic_iterable_impl(const basic_iterable_impl&) = default;
    LZ_CONSTEXPR_CXX_14 basic_iterable_impl& operator=(const basic_iterable_impl&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr basic_iterable_impl()
        requires(std::default_initializable<iterator_type> && std::default_initializable<sentinel_type>)
    = default;

#else

    template<class B = decltype(_begin),
             class = enable_if_t<std::is_default_constructible<B>::value && std::is_default_constructible<sentinel_type>::value>>
    constexpr basic_iterable_impl() noexcept(std::is_nothrow_default_constructible<B>::value &&
                                             std::is_nothrow_default_constructible<sentinel_type>::value) {
    }

#endif

    template<class Iterable>
    explicit constexpr basic_iterable_impl(Iterable&& iterable) :
        basic_iterable_impl{ detail::begin(iterable), detail::end(iterable) } {
    }

    constexpr basic_iterable_impl(iterator_type begin, sentinel_type end) : _begin{ std::move(begin) }, _end{ std::move(end) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(is_ra_tag_v<typename traits::iterator_category>)
    {
        LZ_ASSERT(_end - _begin >= 0, "Incompatible iterator");
        return static_cast<size_t>(_end - _begin);
    }

#else

    template<class Tag = typename traits::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra_tag<Tag>::value, size_t> size() const {
        LZ_ASSERT(_end - _begin >= 0, "Incompatible iterator");
        return static_cast<size_t>(_end - _begin);
    }

#endif

    LZ_NODISCARD constexpr iterator begin() const {
        return _begin;
    }

    LZ_NODISCARD constexpr sentinel end() const {
        return _end;
    }
};

template<class I, class S = I>
class sized_iterable_impl : public lazy_view {
    // we decay I to decay it to a pointer, if it is an array
    using iterator_type = typename std::decay<I>::type;
    using sentinel_type = typename std::decay<S>::type;

    I _begin;
    S _end;
    size_t _size{};

    friend struct common_adaptor;

    constexpr sized_iterable_impl(iterator_type begin, sentinel_type end, size_t size) :
        _begin{ std::move(begin) },
        _end{ std::move(end) },
        _size{ size } {
    }

public:
    using iterator = I;
    using const_iterator = iterator_type;
    using value_type = val_t<iterator_type>;
    using sentinel = S;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr sized_iterable_impl()
        requires(std::default_initializable<iterator_type> && std::default_initializable<sentinel_type>)
    = default;

#else

    template<class B = decltype(_begin),
             class = enable_if_t<std::is_default_constructible<B>::value && std::is_default_constructible<sentinel_type>::value>>
    constexpr sized_iterable_impl() noexcept(std::is_nothrow_default_constructible<B>::value &&
                                             std::is_nothrow_default_constructible<sentinel_type>::value) {
    }

#endif

    constexpr sized_iterable_impl(iterator_type begin, sentinel_type end) :
        _begin{ std::move(begin) },
        _end{ std::move(end) },
        _size{ static_cast<size_t>(_end - _begin) } {
    }

    template<class Iterable>
    explicit constexpr sized_iterable_impl(Iterable&& iterable) :
        _begin{ detail::begin(iterable) },
        _end{ detail::end(iterable) },
        _size{ lz::size(iterable) } {
    }

    LZ_NODISCARD constexpr size_t size() const noexcept {
        return _size;
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return _begin;
    }

    LZ_NODISCARD constexpr sentinel end() const {
        return _end;
    }
};
} // namespace detail
} // namespace lz

// clang-format off
LZ_MODULE_EXPORT namespace lz {
/**
 * @brief A class that can be converted to any container. It only contains the iterators and
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops,
 * etc... It contains the size of the iterable.
 * @tparam It The iterator type.
 * @tparam S The sentinel type.
 */
template<class It, class S = It>
using sized_iterable = detail::sized_iterable_impl<It, S>;

/**
 * @brief A class that can be converted to any container. It only contains the iterators and
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops,
 * etc... It *may* contain the size of the iterable, depending on the iterator category (needs to be random access).
 * @tparam It The iterator type.
 * @tparam S The sentinel type.
 */
template<class It, class S = It>
using basic_iterable = detail::basic_iterable_impl<It, S>;

/**
 * @brief Creates a basic_iterable from an iterator and a sentinel. It can be used to create an iterable from a pair of
 * iterators.
 *
 * @param begin The begin iterator.
 * @param end The end sentinel.
 * @return A basic_iterable object
 */
template<class I, class S>
LZ_NODISCARD constexpr basic_iterable<I, S> make_basic_iterable(I begin, S end) {
    return { std::move(begin), std::move(end) };
}

} // namespace lz

#endif
