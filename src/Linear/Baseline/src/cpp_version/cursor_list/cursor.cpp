#include "cursor.hpp"

#include <stdexcept>

/* START: cursor_list implementation */

template <typename T, std::size_t SpaceSize>
dsaac::CursorList<T, SpaceSize>::CursorList()
{
    nodes_.resize(SpaceSize);
    clear();
}

template <typename T, std::size_t SpaceSize>
bool
dsaac::CursorList<T, SpaceSize>::empty() const
{
    return nodes_[header_].next == npos;
}

template <typename T, std::size_t SpaceSize>
void
dsaac::CursorList<T, SpaceSize>::clear()
{
    if (SpaceSize < 2) {
        throw std::overflow_error("cursor list space is too small");
    }

    free_ = 1;
    for (std::size_t i = 1; i + 1 < SpaceSize; ++i) {
        nodes_[i].next = i + 1;
    }
    nodes_[SpaceSize - 1].next = npos;
    nodes_[header_].next = npos;
}

template <typename T, std::size_t SpaceSize>
void
dsaac::CursorList<T, SpaceSize>::push_back(const T& value)
{
    const auto index = allocate();
    nodes_[index].value = value;
    nodes_[index].next = npos;

    auto current = header_;
    while (nodes_[current].next != npos) {
        current = nodes_[current].next;
    }
    nodes_[current].next = index;
}

template <typename T, std::size_t SpaceSize>
bool
dsaac::CursorList<T, SpaceSize>::contains(const T& value) const
{
    for (auto current = nodes_[header_].next; current != npos; current = nodes_[current].next) {
        if (nodes_[current].value == value) {
            return true;
        }
    }
    return false;
}

template <typename T, std::size_t SpaceSize>
bool
dsaac::CursorList<T, SpaceSize>::erase(const T& value)
{
    auto previous = header_;
    auto current = nodes_[previous].next;
    while (current != npos) {
        if (nodes_[current].value == value) {
            nodes_[previous].next = nodes_[current].next;
            release(current);
            return true;
        }
        previous = current;
        current = nodes_[current].next;
    }
    return false;
}

template <typename T, std::size_t SpaceSize>
std::vector<T>
dsaac::CursorList<T, SpaceSize>::to_vector() const
{
    std::vector<T> result;
    for (auto current = nodes_[header_].next; current != npos; current = nodes_[current].next) {
        result.push_back(nodes_[current].value);
    }
    return result;
}

template <typename T, std::size_t SpaceSize>
std::size_t
dsaac::CursorList<T, SpaceSize>::allocate()
{
    if (free_ == npos) {
        throw std::overflow_error("cursor list space exhausted");
    }
    const auto result = free_;
    free_ = nodes_[free_].next;
    return result;
}

template <typename T, std::size_t SpaceSize>
void
dsaac::CursorList<T, SpaceSize>::release(std::size_t index)
{
    nodes_[index].next = free_;
    free_ = index;
}

/* END */

template class dsaac::CursorList<int, 100>;
