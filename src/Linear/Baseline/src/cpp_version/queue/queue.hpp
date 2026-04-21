#ifndef DSAAC_QUEUE_HPP
#define DSAAC_QUEUE_HPP

#include <cstddef>
#include <vector>

namespace dsaac {

template <typename T>
class CircularQueue {
public:
    explicit CircularQueue(std::size_t capacity);

    bool empty() const;
    bool full() const;
    void push(const T& value);
    void pop();
    const T& front() const;

private:
    std::vector<T> buffer_;
    std::size_t front_ = 0;
    std::size_t back_ = 0;
};

} // namespace dsaac

template <typename T>
using CircularQueue = dsaac::CircularQueue<T>;

#endif
