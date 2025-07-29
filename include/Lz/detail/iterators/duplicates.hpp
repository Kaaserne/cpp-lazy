#ifndef LZ_DUPLICATES_ITERATOR_HPP
#define LZ_DUPLICATES_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <utility>

namespace lz {
namespace detail {

template<class Iterable, class BinaryPredicate, class = void>
class duplicates_iterator;

template<class Iterable, class BinaryPredicate>
class duplicates_iterator<Iterable, BinaryPredicate, enable_if<is_ra<iter_t<Iterable>>::value>>
    : public iterator<duplicates_iterator<Iterable, BinaryPredicate>, std::pair<ref_t<iter_t<Iterable>>, std::size_t>,
                      fake_ptr_proxy<std::pair<ref_t<iter_t<Iterable>>, std::size_t>>, diff_type<iter_t<Iterable>>,
                      common_type<iter_cat_t<iter_t<Iterable>>, std::bidirectional_iterator_tag>, default_sentinel_t> {

    using it = iter_t<Iterable>;
    using traits = std::iterator_traits<it>;

public:
    using value_type = std::pair<typename traits::value_type, std::size_t>;
    using reference = std::pair<typename traits::reference, std::size_t>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

private:
    it _last;
    it _first;
    Iterable _iterable;
    mutable BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        using detail::find_if;
        using std::find_if;

        _last = find_if(_first, std::end(_iterable), [this](typename traits::reference val) { return _compare(*_first, val); });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr duplicates_iterator()
        requires std::default_initializable<it> && std::default_initializable<Iterable> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr duplicates_iterator() noexcept(std::is_nothrow_default_constructible<it>::value &&
                                             std::is_nothrow_default_constructible<Iterable>::value &&
                                             std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif
    template<class I>
    LZ_CONSTEXPR_CXX_14 duplicates_iterator(I&& iterable, it i, BinaryPredicate compare) :
        _last{ std::move(i) },
        _first{ std::move(_last) }, // last is assigned in next()
        _iterable{ std::forward<I>(iterable) },
        _compare{ std::move(compare) } {
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel_t) {
        _first = std::end(_iterable);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { *_first, static_cast<std::size_t>(_last - _first) };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        _first = std::move(_last);
        next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT(_first != std::begin(_iterable), "Cannot decrement begin iterator");
        _last = _first;

        for (--_first; _first != std::begin(_iterable); --_first) {
            auto prev = std::prev(_first);
            if (_compare(*prev, *_first)) {
                return;
            }
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(std::begin(_iterable) == std::begin(other._iterable) && std::end(_iterable) == std::end(other._iterable),
                  "Incompatible iterators");
        return _first == other._first;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel_t) const {
        return _first == std::end(_iterable);
    }
};

template<class Iterable, class BinaryPredicate>
class duplicates_iterator<Iterable, BinaryPredicate, enable_if<!is_ra<iter_t<Iterable>>::value>>
    : public lz::iterator<duplicates_iterator<Iterable, BinaryPredicate>, std::pair<ref_t<iter_t<Iterable>>, std::size_t>,
                          fake_ptr_proxy<std::pair<ref_t<iter_t<Iterable>>, std::size_t>>, diff_type<iter_t<Iterable>>,
                          std::bidirectional_iterator_tag, default_sentinel_t> {

    using it = iter_t<Iterable>;
    using traits = std::iterator_traits<iter_t<Iterable>>;

public:
    using value_type = std::pair<typename traits::value_type, std::size_t>;
    using reference = std::pair<typename traits::reference, std::size_t>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

private:
    it _last;
    it _first;
    Iterable _iterable;
    std::size_t _last_distance;
    mutable BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        using detail::find_if;
        using std::find_if;

        _last_distance = 0;
        _last = find_if(_first, std::end(_iterable), [this](typename traits::reference val) {
            const auto condition = _compare(*_first, val);
            if (!condition) {
                ++_last_distance;
            }
            return condition;
        });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr duplicates_iterator()
        requires std::default_initializable<it> && std::default_initializable<Iterable> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr duplicates_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<Iterable>::value &&
                                             std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 duplicates_iterator(I&& iterable, it i, BinaryPredicate compare) :
        _last{ std::move(i) },
        _first{ std::move(_last) },
        _iterable{ std::forward<I>(iterable) },
        _compare{ std::move(compare) } {
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel_t) {
        _first = std::end(_iterable);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { *_first, _last_distance };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        _first = std::move(_last);
        next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT(_first != std::begin(_iterable), "Cannot decrement before beginning of iterable");
        _last_distance = 1;
        _last = _first;

        for (--_first; _first != std::begin(_iterable); --_first, ++_last_distance) {
            const auto prev = std::prev(_first);
            if (_compare(*prev, *_first)) {
                return;
            }
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(std::begin(_iterable) == std::begin(other._iterable) && std::end(_iterable) == std::end(other._iterable),
                  "Incompatible iterators");
        return _first == other._first;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel_t) const {
        return _first == std::end(_iterable);
    }
};

} // namespace detail
} // namespace lz

#endif
