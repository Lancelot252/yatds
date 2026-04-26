#include "stackar.hpp"

#include <stdexcept>

/* START: array_stack implementation */

template <typename T>
dsaac::ArrayStack<T>::ArrayStack(std::size_t capacity)
{
    values_.reserve(capacity);
}

template <typename T>
bool
dsaac::ArrayStack<T>::empty() const
{
    return values_.empty();
}

template <typename T>
bool
dsaac::ArrayStack<T>::full() const
{
    return values_.size() == values_.capacity();
}

template <typename T>
void
dsaac::ArrayStack<T>::push(const T& value)
{
    if (full()) {
        throw std::overflow_error("stack is full");
    }
    values_.push_back(value);
}

template <typename T>
void
dsaac::ArrayStack<T>::pop()
{
    if (empty()) {
        throw std::underflow_error("stack is empty");
    }
    values_.pop_back();
}

template <typename T>
const T&
dsaac::ArrayStack<T>::top() const
{
    if (empty()) {
        throw std::underflow_error("stack is empty");
    }
    return values_.back();
}

/* END */

template class dsaac::ArrayStack<int>;
