#pragma once
#include <cstddef>
#include <string>
#include <vector>

// 双向链表：用于表示彩球轨道。
// 使用头尾哨兵节点，简化边界插入/删除逻辑。
// 支持 O(1) 任意位置插入/删除（已持有节点指针），O(N) 按下标定位。
template <typename T>
class LinkedList {
public:
    struct Node {
        T value;
        Node* prev;
        Node* next;
        Node() : value(), prev(nullptr), next(nullptr) {}
        Node(const T& v) : value(v), prev(nullptr), next(nullptr) {}
    };

    LinkedList() : count(0) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
    }

    LinkedList(const LinkedList& other) : count(0) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
        for (Node* p = other.head->next; p != other.tail; p = p->next) {
            pushBack(p->value);
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) return *this;
        clear();
        for (Node* p = other.head->next; p != other.tail; p = p->next) {
            pushBack(p->value);
        }
        return *this;
    }

    ~LinkedList() {
        clear();
        delete head;
        delete tail;
    }

    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    void clear() {
        Node* p = head->next;
        while (p != tail) {
            Node* nx = p->next;
            delete p;
            p = nx;
        }
        head->next = tail;
        tail->prev = head;
        count = 0;
    }

    void pushBack(const T& v) {
        insertBefore(tail, v);
    }

    // 在下标 idx 之前插入。idx 范围 [0, size]。
    Node* insertAt(size_t idx, const T& v) {
        if (idx > count) idx = count;
        Node* pos = tail;
        if (idx < count) {
            pos = head->next;
            for (size_t i = 0; i < idx; ++i) pos = pos->next;
        }
        return insertBefore(pos, v);
    }

    Node* insertBefore(Node* pos, const T& v) {
        Node* nd = new Node(v);
        nd->prev = pos->prev;
        nd->next = pos;
        pos->prev->next = nd;
        pos->prev = nd;
        ++count;
        return nd;
    }

    // 删除给定节点。pos 必须不是哨兵。
    void remove(Node* pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        delete pos;
        --count;
    }

    Node* nodeAt(size_t idx) const {
        if (idx >= count) return nullptr;
        Node* p = head->next;
        for (size_t i = 0; i < idx; ++i) p = p->next;
        return p;
    }

    Node* front() const { return count ? head->next : nullptr; }
    Node* back() const { return count ? tail->prev : nullptr; }
    Node* sentinelHead() const { return head; }
    Node* sentinelTail() const { return tail; }

    std::vector<T> toVector() const {
        std::vector<T> out;
        out.reserve(count);
        for (Node* p = head->next; p != tail; p = p->next) out.push_back(p->value);
        return out;
    }

private:
    Node* head;
    Node* tail;
    size_t count;
};
