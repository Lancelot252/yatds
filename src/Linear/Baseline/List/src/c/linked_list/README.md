# 链表

本组任务是完成带头结点的单链表。接口定义在 `list.h`，主要实现写在 `list.c`，`testlist.c` 提供基础运行检查。

## 需要完成的内容

- 在 `list.c` 中定义链表结点 `struct Node`，结点需要保存 `ElementType Element` 和后继指针 `Next`。
- 实现 `MakeEmpty`：创建或清空一个带头结点的空链表，内存申请失败时调用 `FatalError`。
- 实现状态判断函数 `IsEmpty` 和 `IsLast`。
- 实现查找相关函数 `Find`、`FindPrevious`、`Header`、`First`、`Advance`、`Retrieve`。
- 实现 `Insert`：在合法位置 `P` 之后插入新元素，并正确维护后继指针。
- 实现 `Delete`：删除第一个值为 `X` 的结点；若不存在该元素，不应破坏链表。
- 实现 `DeleteList`：释放除头结点外的全部数据结点，并把链表恢复为空表。

## 行为要求

- 链表采用头结点实现，`List` 指向头结点，首个有效元素位于 `L->Next`。
- `Find` 找不到元素时返回 `NULL`。
- `FindPrevious` 找不到元素时返回最后一个结点，使 `Delete` 可以通过 `IsLast` 判断是否需要删除。
- 所有通过 `malloc` 创建的数据结点都需要在删除或清空时释放。

## 验证方式

运行 `testlist.c`，检查输出结果是否符合预期。
