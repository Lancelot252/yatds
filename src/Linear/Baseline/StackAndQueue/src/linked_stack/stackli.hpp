#ifndef DSAAC_LINKED_STACK_HPP
#define DSAAC_LINKED_STACK_HPP

#include <forward_list>

namespace dsaac {

template <typename T>
class LinkedStack {
public:
    bool empty() const;
    void push(const T& value);
    void pop();
    const T& top() const;

private:
    std::forward_list<T> values_;
};

} // namespace dsaac

template <typename T>
using LinkedStack = dsaac::LinkedStack<T>;

#endif
