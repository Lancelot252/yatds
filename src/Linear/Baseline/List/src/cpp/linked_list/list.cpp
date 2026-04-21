#include "list.hpp"

/* START: linked_list implementation */

template <typename T>
dsaac::LinkedList<T>::LinkedList()
    : header_(new Node{})
{
}

template <typename T>
dsaac::LinkedList<T>::LinkedList(const LinkedList& rhs)
    : LinkedList()
{
    copy_from(rhs);
}

template <typename T>
dsaac::LinkedList<T>&
dsaac::LinkedList<T>::operator=(const LinkedList& rhs)
{
    if (this != &rhs) {
        clear();
        copy_from(rhs);
    }
    return *this;
}

template <typename T>
dsaac::LinkedList<T>::~LinkedList()
{
    clear();
    delete header_;
}

template <typename T>
bool
dsaac::LinkedList<T>::empty() const
{
    return header_->next == nullptr;
}

template <typename T>
void
dsaac::LinkedList<T>::clear()
{
    Node* current = header_->next;
    header_->next = nullptr;

    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

template <typename T>
void
dsaac::LinkedList<T>::push_back(const T& value)
{
    Node* current = header_;
    while (current->next != nullptr) {
        current = current->next;
    }

    current->next = new Node{value, nullptr};
}

template <typename T>
bool
dsaac::LinkedList<T>::contains(const T& value) const
{
    for (Node* current = header_->next; current != nullptr; current = current->next) {
        if (current->element == value) {
            return true;
        }
    }
    return false;
}

template <typename T>
bool
dsaac::LinkedList<T>::erase(const T& value)
{
    Node* previous = header_;
    Node* current = header_->next;

    while (current != nullptr) {
        if (current->element == value) {
            previous->next = current->next;
            delete current;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

template <typename T>
std::vector<T>
dsaac::LinkedList<T>::to_vector() const
{
    std::vector<T> result;
    for (Node* current = header_->next; current != nullptr; current = current->next) {
        result.push_back(current->element);
    }
    return result;
}

template <typename T>
void
dsaac::LinkedList<T>::copy_from(const LinkedList& rhs)
{
    Node* tail = header_;
    for (Node* current = rhs.header_->next; current != nullptr; current = current->next) {
        tail->next = new Node{current->element, nullptr};
        tail = tail->next;
    }
}

/* END */

template class dsaac::LinkedList<int>;
