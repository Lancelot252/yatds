# 链表

本组任务是完成 C++ 版单链表封装。`list.hpp` 声明并导出 `LinkedList<T>`，`list.cpp` 标示并实现对应成员函数，`testlist.cpp` 提供基础运行检查。

## 需要完成的内容

- 在 `dsaac::LinkedList<T>` 中使用经典 `struct Node` 结点和指针维护链表存储，不使用 `std::list`、`std::forward_list` 等 STL 链表容器作为内部结构。
- 实现构造函数、析构函数、拷贝构造和拷贝赋值，正确管理动态申请的结点内存。
- 实现 `empty()`：判断链表是否为空。
- 实现 `clear()`：删除全部元素。
- 实现 `push_back(const T&)`：把元素追加到链表末尾，并保持原有顺序。
- 实现 `contains(const T&)`：判断元素是否存在。
- 实现 `erase(const T&)`：删除第一个匹配元素，删除成功返回 `true`，未找到返回 `false`。
- 实现 `to_vector()`：按链表顺序返回所有元素，便于测试和输出。
- 保持 `list.hpp` 中的 `LinkedList<T>` 别名可用，保证测试代码不需要直接写 `dsaac::LinkedList<T>`。
- 在 `list.cpp` 中用 `/* START: linked_list implementation */` 和 `/* END */` 标出主要实现部分。

## 行为要求

- `push_back` 后，`to_vector()` 的顺序应与插入顺序一致。
- `erase` 只删除第一个匹配值，不应影响其他元素。
- 空链表调用 `clear`、`contains`、`erase` 应保持稳定，不应崩溃。
- 析构或清空链表时必须释放所有数据结点。

## 验证方式

运行 `testlist.cpp`