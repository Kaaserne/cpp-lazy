#pragma once

#ifndef LZ_ANY_VIEW_HPP
#define LZ_ANY_VIEW_HPP

#include <Lz/detail/iterators/any_iterable/any_iterator_impl.hpp>
#include <Lz/detail/iterators/iterator_wrapper.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/procs/chain.hpp>
#include <Lz/traits/lazy_view.hpp>

LZ_MODULE_EXPORT namespace lz {
/**
 * @brief Class that can contain any type of view. For example: a container or another view. Use this when you cannot use
 * `auto`. Please be aware that this implementation uses type erasure and therefore is a little bit slower than using
 * `auto`/specifying the "actual" type. For e.g.
 * ```cpp
 * // Preferred:
 * lz::filter_iterable<std::vector<int>::iterator, lambdafunction> f = lz::filter( // stuff...
 * // Or
 * auto f = lz::filter( // stuff
 * // Versus (slower):
 * lz::any_iterable<int> f = lz::filter( // stuff...
 * // or
 * auto f = lz::make_any_iterable(lz::filter( // stuff...
 * ```
 *
 * @tparam T The value_type of the iterator. For example: std::vector<int>::value_type = int, which requires a
 * lz::any_iterable<int>
 * @tparam Reference The reference of the iterator. In most cases T&, but with generative iterators it's oftentimes just T.
 * @tparam IterCat The iterator category. `std::forward_iterator_tag` by default.
 * @tparam DiffType The difference_type. It is used for the return type of iterator - iterator
 */
template<class T, class Reference = T&, class IterCat = std::forward_iterator_tag, class DiffType = std::ptrdiff_t>
class any_iterable : public lazy_view {
private:
    using it = detail::iterator_wrapper<T, Reference, IterCat, DiffType>;

    template<class Iterable>
    using any_iter_impl = detail::any_iterator_impl<iter_t<Iterable>, sentinel_t<Iterable>, T, Reference, IterCat, DiffType>;

    it _begin;
    it _end;

#ifndef LZ_HAS_CONCEPTS

    // One of them fits, the other does not
        template<class Iterable>
        any_iterable(Iterable&& iterable,
                     detail::enable_if_t<
                         (sizeof(iter_t<Iterable>) > it::SBO_SIZE) || (sizeof(sentinel_t<Iterable>) > it::SBO_SIZE), int>) :
            _begin{ detail::make_unique<any_iter_impl<Iterable>>(detail::begin(iterable)) },
            _end{ detail::make_unique<any_iter_impl<Iterable>>(detail::end(iterable)) } {
        }

    // Both fit
    template<class Iterable>
    any_iterable(
        Iterable&& iterable,
        detail::enable_if_t<(sizeof(iter_t<Iterable>) <= it::SBO_SIZE) && (sizeof(sentinel_t<Iterable>) <= it::SBO_SIZE), int>) :
        _begin{ detail::in_place_type_t<any_iter_impl<Iterable>>{}, detail::begin(iterable) },
        _end{ detail::in_place_type_t<any_iter_impl<Iterable>>{}, detail::end(iterable) } {
    }

#endif

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr any_iterable()
        requires(std::default_initializable<it>)
    = default;

    // One of them fits, the other does not

    /**
     * @brief Construct a new any_iterable object
     *
     * @param iterable Any iterable, like a vector, list, etc. Can also be another lz range/view
     */
    template<class Iterable>
        requires((sizeof(iter_t<Iterable>) > it::SBO_SIZE) || (sizeof(sentinel_t<Iterable>) > it::SBO_SIZE))
    any_iterable(Iterable&& iterable) :
        _begin{ detail::make_unique<any_iter_impl<Iterable>>(detail::begin(iterable)) },
        _end{ detail::make_unique<any_iter_impl<Iterable>>(detail::end(iterable)) } {
    }

    // Both fit

    /**
     * @brief Construct a new any_iterable object
     *
     * @param iterable Any iterable, like a vector, list, etc. Can also be another lz range/view
     */
    template<class Iterable>
        requires((sizeof(iter_t<Iterable>) <= it::SBO_SIZE) && (sizeof(sentinel_t<Iterable>) <= it::SBO_SIZE))
    any_iterable(Iterable&& iterable) :
        _begin{ detail::in_place_type_t<any_iter_impl<Iterable>>{}, detail::begin(iterable) },
        _end{ detail::in_place_type_t<any_iter_impl<Iterable>>{}, detail::end(iterable) } {
    }

#else

    template<class I = it, class = detail::enable_if_t<std::is_default_constructible<I>::value>>
    constexpr any_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

    /**
     * @brief Construct a new any_iterable object
     *
     * @param iterable Any iterable, like a vector, list, etc. Can also be another lz range/view
     */
    template<class Iterable>
    any_iterable(Iterable&& iterable) : any_iterable(std::forward<Iterable>(iterable), 0) {
    }

#endif

    LZ_NODISCARD it begin() const {
        return _begin;
    }

    LZ_NODISCARD it end() const {
        return _end;
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(detail::is_ra_tag_v<IterCat>)
    {
        return static_cast<size_t>(std::distance(_begin, _end));
    }

#else

    template<class I = IterCat>
    LZ_NODISCARD detail::enable_if_t<detail::is_ra_tag<I>::value, size_t> size() const {
        return static_cast<size_t>(std::distance(_begin, _end));
    }

#endif
};

/**
 * Creates an any_iterable object from an iterable. This is useful when you cannot use `auto` or when you want to store
 * different types of views in a container.
 *
 * @param iterable The iterable to create an any_iterable from.
 * @return The any_iterable object.
 */
template<class Iterable>
any_iterable<detail::val_iterable_t<Iterable>, detail::ref_iterable_t<Iterable>, detail::iter_cat_iterable_t<Iterable>,
             detail::diff_iterable_t<Iterable>>
make_any_iterable(Iterable && iterable) {
    return any_iterable<detail::val_iterable_t<Iterable>, detail::ref_iterable_t<Iterable>, detail::iter_cat_iterable_t<Iterable>,
                        detail::diff_iterable_t<Iterable>>{ std::forward<Iterable>(iterable) };
}

} // namespace lz

#endif // LZ_ANY_VIEW_HPP
