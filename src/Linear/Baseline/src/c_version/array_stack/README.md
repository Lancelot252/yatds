# 数组栈

本组任务是完成基于动态数组的栈。接口定义在 `stackar.h`，主要实现写在 `stackar.c`，`teststka.c` 提供基础运行检查。

## 需要完成的内容

- 在 `stackar.c` 中定义 `struct StackRecord`，保存栈容量、栈顶下标和元素数组。
- 实现 `CreateStack`：检查最小容量，分配栈记录和数组空间，并初始化为空栈。
- 实现 `DisposeStack`：释放数组空间和栈记录。
- 实现 `MakeEmpty`：把栈顶下标恢复为 `EmptyTOS`。
- 实现 `IsEmpty` 和 `IsFull`。
- 实现 `Push`：满栈时报错，否则递增栈顶并写入元素。
- 实现 `Top`：返回栈顶元素但不弹出。
- 实现 `Pop`：弹出栈顶元素。
- 实现 `TopAndPop`：返回栈顶元素并同时弹出。

## 行为要求

- 空栈的栈顶下标为 `-1`。
- `CreateStack` 的容量小于 `MinStackSize` 时应调用 `Error`。
- 满栈继续 `Push`、空栈调用 `Top`、`Pop` 或 `TopAndPop` 时都应报错。
- 入栈后出栈顺序必须满足后进先出。

## 验证方式

```sh
cc -std=gnu89 stackar.c teststka.c -o /tmp/teststka
/tmp/teststka
```
