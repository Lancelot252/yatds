# 游标链表

本组任务是完成使用静态数组模拟指针的游标链表。接口定义在 `cursor.h`，主要实现写在 `cursor.c`，`testcurs.c` 提供基础运行检查。

## 需要完成的内容

- 在 `cursor.c` 中定义 `struct Node` 和全局 `CursorSpace[SpaceSize]`，每个数组单元保存元素值和下一个游标。
- 实现 `InitializeCursorSpace`：把数组下标串成空闲链表，`CursorSpace[0].Next` 作为空闲表头。
- 实现内部辅助函数 `CursorAlloc` 和 `CursorFree`，分别从空闲表取出结点、把结点归还空闲表。
- 实现 `MakeEmpty`：申请一个头结点游标，并把链表初始化为空。
- 实现状态判断函数 `IsEmpty` 和 `IsLast`。
- 实现查找相关函数 `Find`、`FindPrevious`、`Header`、`First`、`Advance`、`Retrieve`。
- 实现 `Insert`：从空闲表分配结点，在合法位置 `P` 之后插入。
- 实现 `Delete` 和 `DeleteList`：删除结点时必须调用 `CursorFree` 回收游标。

## 行为要求

- 游标值 `0` 表示 `NULL`，同时 `CursorSpace[0]` 用作空闲表头，不存放业务元素。
- 使用本结构前必须先调用 `InitializeCursorSpace`。
- `Find` 找不到元素时返回 `0`。
- 当空闲空间耗尽时，插入或建表应调用 `FatalError` 报错。

## 验证方式

```sh
cc -std=gnu89 cursor.c testcurs.c -o /tmp/testcurs
/tmp/testcurs
```
