# 队列

本组任务是完成 C++ 版循环队列。`queue.hpp` 声明并导出 `CircularQueue<T>`，`queue.cpp` 标示并实现对应成员函数，`testque.cpp` 提供基础运行检查。

## 需要完成的内容

- 在 `dsaac::CircularQueue<T>` 中维护循环缓冲区、队头下标和队尾下标。
- 实现构造函数 `CircularQueue(std::size_t capacity)`：创建可容纳指定数量元素的队列。
- 实现 `empty()`：判断队列是否为空。
- 实现 `full()`：判断队列是否已满。
- 实现 `push(const T&)`：满队列时报错，否则在队尾写入元素并推进队尾。
- 实现 `pop()`：空队列时报错，否则推进队头。
- 实现 `front() const`：返回队头元素引用，但不出队。
- 保持 `queue.hpp` 中的 `CircularQueue<T>` 别名可用。
- 在 `queue.cpp` 中用 `/* START: queue implementation */` 和 `/* END */` 标出主要实现部分。

## 行为要求

- 队列操作必须满足先进先出。
- 下标推进需要在到达缓冲区末尾时回绕到开头。
- 可以使用额外一个缓冲槽区分空队列和满队列。
- 空队列调用 `front` 或 `pop` 时应抛出或报告下溢错误。
- 满队列继续 `push` 时应抛出或报告溢出错误。

## 验证方式

```sh
c++ -std=c++17 queue.cpp testque.cpp -o /tmp/testque
/tmp/testque
```
