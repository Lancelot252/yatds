#ifndef DSAAC_LINKED_LIST_HPP
#define DSAAC_LINKED_LIST_HPP

#include <vector>

namespace dsaac {

template <typename T>
class LinkedList {
public:
    using value_type = T;

    LinkedList();// 默认构造函数
    LinkedList(const LinkedList& rhs);// 拷贝构造函数
    LinkedList& operator=(const LinkedList& rhs);// 赋值运算符
    ~LinkedList();// 析构函数

    bool empty() const;// 判断链表是否为空
    void clear();// 清空链表
    void push_back(const T& value);// 在链表末尾添加元素
    bool contains(const T& value) const;// 判断链表是否包含某个元素
    bool erase(const T& value);// 从链表中删除某个元素
    std::vector<T> to_vector() const;// 将链表转换为向量

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
