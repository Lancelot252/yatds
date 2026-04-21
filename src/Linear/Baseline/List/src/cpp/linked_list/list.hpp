#ifndef DSAAC_LINKED_LIST_HPP
#define DSAAC_LINKED_LIST_HPP

#include <vector>

namespace dsaac {

template <typename T>
class LinkedList {
public:
    using value_type = T;

    LinkedList();
    LinkedList(const LinkedList& rhs);
    LinkedList& operator=(const LinkedList& rhs);
    ~LinkedList();

    bool empty() const;
    void clear();
    void push_back(const T& value);
    bool contains(const T& value) const;
    bool erase(const T& value);
    std::vector<T> to_vector() const;

private:
    struct Node {
        T element{};
        Node* next = nullptr;
    };

    Node* header_;

    void copy_from(const LinkedList& rhs);
};

} // namespace dsaac

template <typename T>
using LinkedList = dsaac::LinkedList<T>;

#endif
