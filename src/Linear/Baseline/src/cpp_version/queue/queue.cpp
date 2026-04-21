#include "queue.hpp"

#include <stdexcept>

/* START: queue implementation */

template <typename T>
dsaac::CircularQueue<T>::CircularQueue(std::size_t capacity)
    : buffer_(capacity + 1)
{
}

template <typename T>
bool
dsaac::CircularQueue<T>::empty() const
{
    return front_ == back_;
}

template <typename T>
bool
dsaac::CircularQueue<T>::full() const
{
    return (back_ + 1) % buffer_.size() == front_;
}

template <typename T>
void
dsaac::CircularQueue<T>::push(const T& value)
{
    if (full()) {
        throw std::overflow_error("queue is full");
    }
    buffer_[back_] = value;
    back_ = (back_ + 1) % buffer_.size();
}

template <typename T>
void
dsaac::CircularQueue<T>::pop()
{
    if (empty()) {
        throw std::underflow_error("queue is empty");
    }
    front_ = (front_ + 1) % buffer_.size();
}

template <typename T>
const T&
dsaac::CircularQueue<T>::front() const
{
    if (empty()) {
        throw std::underflow_error("queue is empty");
    }
    return buffer_[front_];
}

/* END */

template class dsaac::CircularQueue<int>;
