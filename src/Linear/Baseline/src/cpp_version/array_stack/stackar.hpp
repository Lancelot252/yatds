#ifndef DSAAC_ARRAY_STACK_HPP
#define DSAAC_ARRAY_STACK_HPP

#include <cstddef>
#include <vector>

namespace dsaac {

template <typename T>
class ArrayStack {
public:
    explicit ArrayStack(std::size_t capacity);

    bool empty() const;
    bool full() const;
    void push(const T& value);
    void pop();
    const T& top() const;

private:
    std::vector<T> values_;
};

} // namespace dsaac

template <typename T>
using ArrayStack = dsaac::ArrayStack<T>;

#endif
