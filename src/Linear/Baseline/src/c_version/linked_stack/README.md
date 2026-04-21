# 链式栈

本组任务是完成链式栈。接口定义在 `stackli.h`，主要实现写在 `stackli.c`，`teststkl.c` 提供基础运行检查。

## 需要完成的内容

- 在 `stackli.c` 中定义栈结点 `struct Node`，结点需要保存 `ElementType Element` 和后继指针 `Next`。
- 实现 `CreateStack`：创建带头结点的空栈。
- 实现 `MakeEmpty`：反复弹出数据结点，直到栈为空。
- 实现 `DisposeStack`：清空栈并释放头结点。
- 实现 `IsEmpty`：判断头结点后是否还有数据结点。
- 实现 `Push`：把新元素插入到头结点之后。
- 实现 `Top`：返回栈顶元素，但不删除结点。
- 实现 `Pop`：删除栈顶结点并释放内存。

## 行为要求

- 栈使用头结点，真正的栈顶是 `S->Next`。
- 空栈调用 `Top` 或 `Pop` 时应通过 `Error` 报错。
- 所有入栈申请的结点都需要在出栈、清空或销毁时释放。
- 入栈后出栈顺序必须满足后进先出。

## 验证方式

```sh
cc -std=gnu89 stackli.c teststkl.c -o /tmp/teststkl
/tmp/teststkl
```
