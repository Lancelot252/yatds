# 队列

本组任务是完成 C++ 版循环队列。`queue.hpp` 声明并导出 `CircularQueue<T>`；请自行创建 `queue.cpp` 完成成员函数，并编写测试程序。

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

## 规定测试样例格式

父级 `StackAndQueue/README.md` 已给出队列 baseline 的统一输入输出格式。循环队列测试程序建议按以下协议扩展：

注：父级 README 中的测试样例仅用于说明输入输出格式，不代表完整测试覆盖范围，也不是唯一评测数据。学生需要根据该格式自行设计并生成更多测试样例，用于覆盖边界情况和自定义扩展功能。

本组队列任务包含性能差异分析要求，测试程序输出必须包含可量化的性能统计数据，例如数据规模、操作次数、运行时间（如 `time_ms`）等。

- 第一行读取 `capacity n`，第二行读取从队首到队尾的 `n` 个初始元素。
- 操作范围：`PUSH x`、`POP`、`FRONT`、`SIZE`、`EMPTY`、`PRINT`。
- 输出约定：`POP`、`FRONT`、`SIZE`、`EMPTY`、`PRINT` 逐行输出；空队列访问输出 `ERROR`；空队列打印输出 `EMPTY`；布尔值输出 `true` 或 `false`。

请自行编写基础接口检查和命令式输入输出测试，覆盖正常操作、空队列、满队列及环形回绕。
