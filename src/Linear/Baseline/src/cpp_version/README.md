# 线性结构

本目录保存线性结构 Baseline 的 C++ 任务代码。每个数据结构任务都在自己的子目录中保存头文件、实现文件和基础测试入口；公共目录只保留跨任务辅助代码。

完成任务时优先阅读对应子目录的 `README.md`，其中列出了本组需要编写的类、成员函数、边界情况和验证命令。

## 子目录

- `linked_list/`: 完成 `dsaac::LinkedList<T>`，支持尾插、查找、删除、清空和转数组。
- `cursor_list/`: 完成 `dsaac::CursorList<T, SpaceSize>`，使用数组游标和空闲表模拟链表。
- `linked_stack/`: 完成 `dsaac::LinkedStack<T>`，支持入栈、出栈、取栈顶和空栈检查。
- `array_stack/`: 完成 `dsaac::ArrayStack<T>`，支持固定容量、满栈检查和栈操作。
- `queue/`: 完成 `dsaac::CircularQueue<T>`，支持循环缓冲区上的队列操作。
- `polynomial/`: 完成 `dsaac::Polynomial<Coeff>`，支持设置系数、查询系数、加法和乘法。
- `common/`: C++ 公共错误处理辅助代码。
