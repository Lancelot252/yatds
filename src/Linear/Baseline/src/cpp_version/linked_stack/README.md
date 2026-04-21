# 链式栈

本组任务是完成 C++ 版链式栈。`stackli.hpp` 声明并导出 `LinkedStack<T>`，`stackli.cpp` 标示并实现对应成员函数，`teststkl.cpp` 提供基础运行检查。

## 需要完成的内容

- 在 `dsaac::LinkedStack<T>` 中维护链式存储，可以使用 `std::forward_list<T>` 或自行实现结点结构。
- 实现 `empty()`：判断栈是否为空。
- 实现 `push(const T&)`：把元素压入栈顶。
- 实现 `pop()`：弹出栈顶元素。
- 实现 `top() const`：返回栈顶元素引用，但不弹出。
- 保持 `stackli.hpp` 中的 `LinkedStack<T>` 别名可用。
- 在 `stackli.cpp` 中用 `/* START: linked_stack implementation */` 和 `/* END */` 标出主要实现部分。

## 行为要求

- 栈操作必须满足后进先出。
- 空栈调用 `top` 或 `pop` 时应抛出或报告下溢错误。
- `top` 不应改变栈内容。
- 连续 `push` 后，测试应按反向插入顺序输出元素。

## 验证方式

```sh
c++ -std=c++17 stackli.cpp teststkl.cpp -o /tmp/teststkl
/tmp/teststkl
```
