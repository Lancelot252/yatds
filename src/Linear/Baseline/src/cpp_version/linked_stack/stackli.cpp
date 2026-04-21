#include "stackli.hpp"

#include <stdexcept>

/* START: linked_stack implementation */

template <typename T>
bool
dsaac::LinkedStack<T>::empty() const
{
    return values_.empty();
}

template <typename T>
void
dsaac::LinkedStack<T>::push(const T& value)
{
    values_.push_front(value);
}

template <typename T>
void
dsaac::LinkedStack<T>::pop()
{
    if (empty()) {
        throw std::underflow_error("stack is empty");
    }
    values_.pop_front();
}

template <typename T>
const T&
dsaac::LinkedStack<T>::top() const
{
    if (empty()) {
        throw std::underflow_error("stack is empty");
    }
    return values_.front();
}

/* END */

template class dsaac::LinkedStack<int>;
