#pragma once

#ifndef LZ_ANY_VIEW_HELPERS_HPP
#define LZ_ANY_VIEW_HELPERS_HPP

#include <Lz/detail/iterators/any_iterable/iterator_base.hpp>
#include <Lz/detail/unique_ptr.hpp>
#include <Lz/detail/variant.hpp>
#include <Lz/iterator_base.hpp>
#include <cstring>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

using std::in_place_type_t;

#else

template<class T>
struct in_place_type_t {
    explicit in_place_type_t() = default;
};

#endif

template<class T, class Reference, class IterCat, class DiffType>
class iterator_wrapper : public iterator<iterator_wrapper<T, Reference, IterCat, DiffType>, Reference, fake_ptr_proxy<Reference>,
                                         DiffType, IterCat> {

    using any_iter_base = iterator_base<Reference, IterCat, DiffType>;

public:
    static constexpr std::size_t SBO_SIZE = 64;

private:
    struct alignas(std::max_align_t) storage {
        char buffer[SBO_SIZE];
    };

    variant<unique_ptr<any_iter_base>, storage> _storage;

    void copy_buf_other(const iterator_wrapper& other) noexcept {
        LZ_ASSERT(other._storage.index() == 1, "Invalid storage index");
        using std::get;
        _storage.template emplace<1>();
        std::memcpy(static_cast<void*>(&get<1>(_storage)), static_cast<const void*>(&get<1>(other._storage)), sizeof(storage));
    }

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = IterCat;

    constexpr iterator_wrapper() noexcept = default;

    template<class Impl, class... Args>
    iterator_wrapper(in_place_type_t<Impl>, Args&&... args) {
        static_assert(sizeof(Impl) <= SBO_SIZE, "Impl too large for SBO");
        using std::get;
        _storage.template emplace<1>();
        ::new (static_cast<void*>(&get<1>(_storage))) Impl(std::forward<Args>(args)...);
    }

    iterator_wrapper(const detail::unique_ptr<any_iter_base>& ptr) : _storage{ ptr->clone() } {
    }

    iterator_wrapper(detail::unique_ptr<any_iter_base>&& ptr) noexcept : _storage{ std::move(ptr) } {
    }

    iterator_wrapper(const iterator_wrapper& other) {
        using std::get;
        if (other._storage.index() == 0) {
            _storage = get<0>(other._storage)->clone();
        }
        else {
            copy_buf_other(other);
        }
    }

    iterator_wrapper(iterator_wrapper&& other) noexcept {
        using std::get;
        if (other._storage.index() == 0) {
            _storage = std::move(get<0>(other._storage));
        }
        else {
            copy_buf_other(other);
        }
    }

    iterator_wrapper& operator=(const iterator_wrapper& other) {
        if (this != &other) {
            using std::get;
            if (other._storage.index() == 0) {
                _storage = get<0>(other._storage)->clone();
            }
            else {
                copy_buf_other(other);
            }
        }
        return *this;
    }

    iterator_wrapper& operator=(iterator_wrapper&& other) noexcept {
        if (this != &other) {
            using std::get;
            if (other._storage.index() == 0) {
                _storage = std::move(get<0>(other._storage));
            }
            else {
                copy_buf_other(other);
            }
        }
        return *this;
    }

    reference dereference() const {
        using std::get;
        if (_storage.index() == 0) {
            return get<0>(_storage)->dereference();
        }
        LZ_ASSERT(_storage.index() == 1, "Invalid storage index");
        return reinterpret_cast<const any_iter_base&>(get<1>(_storage)).dereference();
    }

    reference dereference() {
        using std::get;
        if (_storage.index() == 0) {
            return get<0>(_storage)->dereference();
        }
        LZ_ASSERT(_storage.index() == 1, "Invalid storage index");
        return reinterpret_cast<any_iter_base&>(get<1>(_storage)).dereference();
    }

    pointer arrow() const {
        using std::get;
        if (_storage.index() == 0) {
            return get<0>(_storage)->arrow();
        }
        LZ_ASSERT(_storage.index() == 1, "Invalid storage index");
        return reinterpret_cast<const any_iter_base&>(get<1>(_storage)).arrow();
    }

    pointer arrow() {
        using std::get;
        if (_storage.index() == 0) {
            return get<0>(_storage)->arrow();
        }
        LZ_ASSERT(_storage.index() == 1, "Invalid storage index");
        return reinterpret_cast<any_iter_base&>(get<1>(_storage)).arrow();
    }

    void increment() {
        using std::get;
        if (_storage.index() == 0) {
            get<0>(_storage)->increment();
            return;
        }
        LZ_ASSERT(_storage.index() == 1, "Invalid storage index");
        reinterpret_cast<any_iter_base&>(get<1>(_storage)).increment();
    }

    void decrement() {
        using std::get;
        if (_storage.index() == 0) {
            get<0>(_storage)->decrement();
            return;
        }
        LZ_ASSERT(_storage.index() == 1, "Invalid storage index");
        return reinterpret_cast<any_iter_base&>(get<1>(_storage)).decrement();
    }

    bool eq(const iterator_wrapper& other) const {
        using std::get;

        switch ((_storage.index() << 1) | other._storage.index()) {
        case 0: // (0, 0)
            return get<0>(_storage)->eq(*get<0>(other._storage));
        case 1: // (0, 1)
            return get<0>(_storage)->eq(reinterpret_cast<const any_iter_base&>(get<1>(other._storage)));
        case 2: // (1, 0)
            return reinterpret_cast<const any_iter_base&>(get<1>(_storage)).eq(*get<0>(other._storage));
        case 3: // (1, 1)
            return reinterpret_cast<const any_iter_base&>(get<1>(_storage))
                .eq(reinterpret_cast<const any_iter_base&>(get<1>(other._storage)));
        default:
            LZ_ASSERT(_storage.index() < 2 && other._storage.index() < 2, "Invalid storage index");
            return false;
        }
    }

    void plus_is(const DiffType n) {
        using std::get;
        if (_storage.index() == 0) {
            return get<0>(_storage)->plus_is(n);
        }
        LZ_ASSERT(_storage.index() == 1, "Invalid storage index");
        return reinterpret_cast<any_iter_base&>(get<1>(_storage)).plus_is(n);
    }

    DiffType difference(const iterator_wrapper& other) const {
        using std::get;

        switch ((_storage.index() << 1) | other._storage.index()) {
        case 0: // (0, 0)
            return get<0>(_storage)->difference(*get<0>(other._storage));
        case 1: // (0, 1)
            return get<0>(_storage)->difference(reinterpret_cast<const any_iter_base&>(get<1>(other._storage)));
        case 2: // (1, 0)
            return reinterpret_cast<const any_iter_base&>(get<1>(_storage)).difference(*get<0>(other._storage));
        case 3: // (1, 1)
            return reinterpret_cast<const any_iter_base&>(get<1>(_storage))
                .difference(reinterpret_cast<const any_iter_base&>(get<1>(other._storage)));
        default:
            LZ_ASSERT(_storage.index() < 2 && other._storage.index() < 2, "Invalid storage index");
            return 0;
        }
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_ANY_VIEW_HELPERS_HPP
