# 模式匹配算法 (Pattern Match)

本组任务是完成 C++ 版字符串的模式匹配。`match.hpp` 声明并导出了 `naiveMatch`（朴素匹配）、`getNext`（部分匹配表生成）和 `kmpMatch`（KMP匹配算法），`match.cpp` 标示并实现了对应的基础函数，`test_match.cpp` 提供基础的运行检查。

## 需要完成的内容

- 在 `dsaac::kmpMatch` 中实现完整的 KMP 模式匹配算法（可直接调用同文件下的 `dsaac::getNext` 函数）。
- 在现有基础上，实现更优化的 `nextval` 数组求解算法，并以此写出优化版的 KMP 匹配。
- 构造时间复杂度存在明显差异的极端测试用例（例如：主串与模式串大多相同但在末尾才失配），通过 `test_match.cpp` 比较不同模式匹配算法带来的性能差异。
- 在 `match.cpp` 中可以通过 `/* START: pattern_match implementation */` 和 `/* END */` 标出主要实现部分，保留统一规范格式。

## 行为要求

- 涉及到长度的返回值/传参请妥善处理，推荐强制转型保证安全退出或使用 `std::size_t` / `int` 规范处理。
- 匹配成功应返回主串中初次匹配成功所在的首字母索引，失败应返回 `-1`。

## 验证方式

```sh
g++ -std=c++17 match.cpp test_match.cpp -o /tmp/test_match
/tmp/test_match
```