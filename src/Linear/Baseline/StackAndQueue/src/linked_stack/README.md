# 链式栈

本组任务是完成 C++ 版链式栈。`stackli.hpp` 声明并导出 `LinkedStack<T>`；请自行创建 `stackli.cpp` 完成成员函数，并编写测试程序。

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

## 规定测试样例格式

父级 `StackAndQueue/README.md` 已给出栈 baseline 的统一输入输出格式。链式栈测试程序建议按以下协议扩展：

注：父级 README 中的测试样例仅用于说明输入输出格式，不代表完整测试覆盖范围，也不是唯一评测数据。学生需要根据该格式自行设计并生成更多测试样例，用于覆盖边界情况和自定义扩展功能。

本组栈任务包含性能差异分析要求，测试程序输出必须包含可量化的性能统计数据，例如数据规模、操作次数、运行时间（如 `time_ms`）等。

- 第一行读取 `capacity n`，其中链式栈可忽略 `capacity`，但仍需读取以保持格式一致。
- 第二行读取从栈底到栈顶的 `n` 个初始元素。
- 操作范围：`PUSH x`、`POP`、`TOP`、`SIZE`、`EMPTY`、`PRINT`。
- 输出约定：`POP`、`TOP`、`SIZE`、`EMPTY`、`PRINT` 逐行输出；空栈访问输出 `ERROR`；空栈打印输出 `EMPTY`；布尔值输出 `true` 或 `false`。

请自行编写基础接口检查和命令式输入输出测试，覆盖正常操作、空栈和连续入栈出栈。
