#pragma once
#include <cstddef>
#include <vector>

// 基于单链表实现的队列：用于存放待发射的彩球。
// FIFO 行为：enqueue 在尾部加入，dequeue 从头部弹出。
// 额外提供 sendToBack：把队首移到队尾（用作"跳过当前球"的道具）。
template <typename T>
class Queue {
public:
    Queue() : head(nullptr), tail(nullptr), count(0) {}

    Queue(const Queue& other) : head(nullptr), tail(nullptr), count(0) {
        for (Node* p = other.head; p; p = p->next) enqueue(p->value);
    }

    Queue& operator=(const Queue& other) {
        if (this == &other) return *this;
        clear();
        for (Node* p = other.head; p; p = p->next) enqueue(p->value);
        return *this;
    }

    ~Queue() { clear(); }

    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    void clear() {
        while (head) {
            Node* nx = head->next;
            delete head;
            head = nx;
        }
        tail = nullptr;
        count = 0;
    }

    void enqueue(const T& v) {
        Node* nd = new Node(v);
        if (!tail) {
            head = tail = nd;
        } else {
            tail->next = nd;
            tail = nd;
        }
        ++count;
    }

    bool dequeue(T& out) {
        if (!head) return false;
        Node* nd = head;
        out = nd->value;
        head = head->next;
        if (!head) tail = nullptr;
        delete nd;
        --count;
        return true;
    }

    bool peek(T& out) const {
        if (!head) return false;
        out = head->value;
        return true;
    }

    // 把队首元素挪到队尾。空队列时返回 false。
    bool sendToBack() {
        if (count <= 1) return count == 1;
        Node* nd = head;
        head = head->next;
        nd->next = nullptr;
        tail->next = nd;
        tail = nd;
        return true;
    }

    std::vector<T> toVector() const {
        std::vector<T> out;
        out.reserve(count);
        for (Node* p = head; p; p = p->next) out.push_back(p->value);
        return out;
    }

private:
    struct Node {
        T value;
        Node* next;
        Node(const T& v) : value(v), next(nullptr) {}
    };
    Node* head;
    Node* tail;
    size_t count;
};
