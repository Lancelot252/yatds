# 查找算法 Baseline

本部分要求完成静态查找和动态查找结构的基础实现。每个算法目录包含面向学生的任务说明和接口头文件；算法实现文件与测试数据由学生按照任务说明自行完成。

## 实验模块

| 分类 | 目录 | 内容 |
| --- | --- | --- |
| 静态查找 | `src/StaticSearch/SequentialSearch/` | 顺序查找与哨兵查找 |
| 静态查找 | `src/StaticSearch/OrderedSearch/` | 二分、插值和斐波那契查找 |
| 静态查找 | `src/StaticSearch/StaticTreeTable/` | 静态树表构造与查找 |
| 静态查找 | `src/StaticSearch/IndexedSequentialSearch/` | 索引顺序表与分块查找 |
| 动态查找 | `src/DynamicSearch/BSTAVL/` | BST 与 AVL 树 |
| 动态查找 | `src/DynamicSearch/BTreeBPlusTree/` | B-树与 B+树 |
| 动态查找 | `src/DynamicSearch/Trie/` | Trie 字典树 |

## 提交约定

- 保持头文件中给定的公开接口不变。
- 在对应目录中完成算法 `.cpp` 文件，并自行编写测试入口。
- 使用 C++17 编译，不依赖第三方库。
- 输入输出行为遵循各模块 `README.md` 中的规定，学生应自行设计正常、边界与失败场景的测试数据。
- 除规定结果外，不输出额外提示信息。
- 实验报告应说明算法原理、复杂度、边界情况和测试设计。
