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

## 规定测试样例格式

父级 `StackAndQueue/README.md` 已给出栈 baseline 的统一输入输出格式。数组栈测试程序建议按以下协议扩展：

注：父级 README 中的测试样例仅用于说明输入输出格式，不代表完整测试覆盖范围，也不是唯一评测数据。学生需要根据该格式自行设计并生成更多测试样例，用于覆盖边界情况和自定义扩展功能。

本组栈任务包含性能差异分析要求，测试程序输出必须包含可量化的性能统计数据，例如数据规模、操作次数、运行时间（如 `time_ms`）等。

- 第一行读取 `capacity n`，第二行读取从栈底到栈顶的 `n` 个初始元素。
- 操作范围：`PUSH x`、`POP`、`TOP`、`SIZE`、`EMPTY`、`PRINT`。
- 输出约定：`POP`、`TOP`、`SIZE`、`EMPTY`、`PRINT` 逐行输出；空栈访问输出 `ERROR`；空栈打印输出 `EMPTY`；布尔值输出 `true` 或 `false`。

仓库给出的 `teststka.cpp` 仍是基础接口检查，不读取标准输入；你的扩展测试应补充上述命令式输入输出格式。
