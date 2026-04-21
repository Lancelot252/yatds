# 数组栈

本组任务是完成 C++ 版数组栈。`stackar.hpp` 声明并导出 `ArrayStack<T>`，`stackar.cpp` 标示并实现对应成员函数，`teststka.cpp` 提供基础运行检查。

## 需要完成的内容

- 在 `dsaac::ArrayStack<T>` 中维护固定容量的顺序存储，可以使用 `std::vector<T>` 并提前保留容量。
- 实现构造函数 `ArrayStack(std::size_t capacity)`：设置最大容量。
- 实现 `empty()`：判断栈是否为空。
- 实现 `full()`：判断栈是否达到最大容量。
- 实现 `push(const T&)`：满栈时报错，否则把元素压入栈顶。
- 实现 `pop()`：空栈时报错，否则弹出栈顶。
- 实现 `top() const`：返回栈顶元素引用，但不弹出。
- 保持 `stackar.hpp` 中的 `ArrayStack<T>` 别名可用。
- 在 `stackar.cpp` 中用 `/* START: array_stack implementation */` 和 `/* END */` 标出主要实现部分。

## 行为要求

- 栈操作必须满足后进先出。
- `full()` 应基于当前元素个数和容量判断。
- 空栈调用 `top` 或 `pop` 时应抛出或报告下溢错误。
- 满栈继续 `push` 时应抛出或报告溢出错误。

## 验证方式

```sh
c++ -std=c++17 stackar.cpp teststka.cpp -o /tmp/teststka
/tmp/teststka
```
