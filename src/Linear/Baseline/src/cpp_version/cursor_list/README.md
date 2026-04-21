# 游标链表

本组任务是完成 C++ 版游标链表。`cursor.hpp` 声明并导出 `CursorList<T, SpaceSize>`，`cursor.cpp` 标示并实现对应成员函数，`testcurs.cpp` 提供基础运行检查。

## 需要完成的内容

- 在 `dsaac::CursorList<T, SpaceSize>` 中维护固定大小的结点数组，每个结点保存元素值和下一个下标。
- 使用下标 `0` 作为头结点，使用特殊值 `npos` 表示空指针。
- 实现空闲表管理：`allocate()` 从空闲链表取结点，`release()` 把结点还回空闲链表。
- 实现 `clear()`：重建空闲链表，并把业务链表清空。
- 实现 `empty()`：判断头结点后是否有数据结点。
- 实现 `push_back(const T&)`：从空闲表分配结点并追加到链表末尾。
- 实现 `contains(const T&)`：遍历业务链表查找元素。
- 实现 `erase(const T&)`：删除第一个匹配元素并回收游标。
- 实现 `to_vector()`：按链表顺序导出全部元素。
- 在 `cursor.cpp` 中用 `/* START: cursor_list implementation */` 和 `/* END */` 标出主要实现部分。

## 行为要求

- 空间耗尽时，`push_back` 应抛出或报告溢出错误。
- 删除元素后，对应数组位置必须回到空闲表，后续插入可以复用。
- `clear` 后链表为空，且可重新插入元素。
- 遍历时不能访问 `npos` 对应的非法数组位置。

## 验证方式

```sh
c++ -std=c++17 cursor.cpp testcurs.cpp -o /tmp/testcurs
/tmp/testcurs
```
