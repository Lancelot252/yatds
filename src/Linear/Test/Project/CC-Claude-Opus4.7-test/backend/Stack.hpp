#pragma once
#include <cstddef>

// 基于单链表的栈：用于存放游戏状态快照，支持撤销。
// LIFO：push 在栈顶加入；pop 从栈顶弹出。
template <typename T>
class Stack {
public:
    Stack() : top_(nullptr), count(0) {}

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    ~Stack() { clear(); }

    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    void clear() {
        while (top_) {
            Node* nx = top_->next;
            delete top_;
            top_ = nx;
        }
        count = 0;
    }

    void push(const T& v) {
        Node* nd = new Node(v);
        nd->next = top_;
        top_ = nd;
        ++count;
    }

    bool pop(T& out) {
        if (!top_) return false;
        Node* nd = top_;
        out = nd->value;
        top_ = nd->next;
        delete nd;
        --count;
        return true;
    }

    bool peek(T& out) const {
        if (!top_) return false;
        out = top_->value;
        return true;
    }

private:
    struct Node {
        T value;
        Node* next;
        Node(const T& v) : value(v), next(nullptr) {}
    };
    Node* top_;
    size_t count;
};
