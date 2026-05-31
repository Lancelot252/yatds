# 顺序存储的字符串 (MyString)

本组任务是完成 C++ 版基于动态数组的自定义字符串类。`mystring.hpp` 声明并导出 `MyString`，`mystring.cpp` 标示并实现了对应的成员函数，`test_mystring.cpp` 提供基础的运行检查。

## 需要完成的内容

- 在 `dsaac::MyString` 中维护字符串序列，包括 `data_` 指针、对应的 `len_` 及动态分配的实际容量 `cap_`。
- 完善 `append(const MyString& other)` 等基础内容：处理内存不足时的动态扩容工作，使得 `MyString` 可以在性能较优的条件下完成频繁的拼接。
- 参考 C++ 的 `<string>` 实现，添加新的基础功能，例如 `find` 查找、`replace` 替换等，至少5个新功能。
- 在 `mystring.cpp` 中可以通过 `/* START: mystring implementation */` 和 `/* END */` 标出主要实现部分，保留统一规范格式。

## 行为要求

- 涉及到内存申请时必须妥善管理内存释放，包括拷贝构造函数与赋值重载的深度拷贝，避免内存泄漏或浅拷贝。
- 非法操作，如超出长度边界的 `substr()` 或 `find()` 应抛出 `std::out_of_range` 等相应的异常报错。

## 验证方式

```sh
g++ -std=c++17 mystring.cpp test_mystring.cpp -o /tmp/test_mystring
/tmp/test_mystring
```

## 规定测试样例格式

父级 `String/README.md` 已给出字符串基础实现 baseline 的统一输入输出格式。扩展测试程序建议按该格式读取初始字符串和操作序列，并输出 `PRINT`、`LENGTH`、`SUBSTR`、`FIND`、`COMPARE` 等查询结果。

注：父级 README 中的测试样例仅用于说明输入输出格式，不代表完整测试覆盖范围，也不是唯一评测数据。学生需要根据该格式自行设计并生成更多测试样例，用于覆盖边界情况和自定义扩展功能。本组任务包含性能差异分析要求，测试程序输出必须包含可量化的性能统计数据，例如数据规模、操作次数、运行时间（如 `time_ms`）等。
