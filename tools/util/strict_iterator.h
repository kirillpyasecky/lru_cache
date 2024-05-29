#pragma once

#include <iterator>
#include <list>
#include <ranges>
#include <vector>

template <std::bidirectional_iterator Iterator>
class StrictIterator {
   public:
    using iterator_type = Iterator;                                    // NOLINT
    using iterator_concept = std::bidirectional_iterator_tag;          // NOLINT
    using iterator_category = std::bidirectional_iterator_tag;         // NOLINT
    using value_type = std::iter_value_t<Iterator>;                    // NOLINT
    using difference_type = std::iter_difference_t<Iterator>;          // NOLINT
    using pointer = typename std::iterator_traits<Iterator>::pointer;  // NOLINT
    using reference = typename std::iter_reference_t<Iterator>;        // NOLINT

    StrictIterator() : is_init_{false} {
    }

    StrictIterator(Iterator first, Iterator current, Iterator last)
        : first_{first}, current_{current}, last_{last}, is_init_{true} {
    }

    StrictIterator& operator++() {
        CheckInit();
        if (current_ == last_) {
            throw std::range_error{"Out of range (right)"};
        }
        ++current_;
        return *this;
    }

    StrictIterator operator++(int) {  // NOLINT(cert-dcl21-cpp)
        CheckInit();
        auto old = *this;
        ++*this;
        return old;
    }

    StrictIterator& operator--() {
        CheckInit();
        if (current_ == first_) {
            throw std::range_error{"Out of range (left)"};
        }
        --current_;
        return *this;
    }

    StrictIterator operator--(int) {  // NOLINT(cert-dcl21-cpp)
        CheckInit();
        auto old = *this;
        --*this;
        return old;
    }

    reference operator*() const {
        CheckInit();
        if (current_ == last_) {
            throw std::range_error{"Dereferencing end of sequence"};
        }
        return *current_;
    }

    iterator_type operator->() const {
        CheckInit();
        return current_;
    }

    [[nodiscard]] Iterator Base() const {
        CheckInit();
        return current_;
    }

    bool operator==(const StrictIterator& r) const = default;

   private:
    Iterator first_, current_, last_;
    bool is_init_;

    void CheckInit() const {
        if (!is_init_) {
            throw std::runtime_error{"Using uninitialized iterator"};
        }
    }
};

template <class T>
concept CanBeRestricted = std::bidirectional_iterator<StrictIterator<T>>;

static_assert(CanBeRestricted<int*>);
static_assert(CanBeRestricted<std::vector<int>::iterator>);
static_assert(CanBeRestricted<std::vector<int>::const_iterator>);
static_assert(CanBeRestricted<std::list<int>::iterator>);
static_assert(CanBeRestricted<std::list<int>::const_iterator>);

using IotaIterator = decltype(std::declval<std::ranges::iota_view<int, int>>().begin());
static_assert(CanBeRestricted<IotaIterator>);

template <class Iterator>
StrictIterator<Iterator> MakeStrict(Iterator first, Iterator current, Iterator last) {
    return {first, current, last};
}
