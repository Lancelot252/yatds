# 队列

本组任务是完成循环数组队列。接口定义在 `queue.h`，主要实现写在 `queue.c`，`testque.c` 提供基础运行检查。

## 需要完成的内容

- 在 `queue.c` 中定义 `struct QueueRecord`，保存容量、队头下标、队尾下标、当前元素个数和元素数组。
- 实现 `CreateQueue`：检查最小容量，分配队列记录和数组空间，并初始化为空队列。
- 实现 `DisposeQueue`：释放数组空间和队列记录。
- 实现 `MakeEmpty`：重置 `Size`、`Front` 和 `Rear`。
- 实现 `IsEmpty` 和 `IsFull`。
- 实现循环下标辅助函数 `Succ`，用于处理数组末尾回绕。
- 实现 `Enqueue`：满队列时报错，否则把元素加入队尾。
- 实现 `Front`：返回队头元素但不出队。
- 实现 `Dequeue`：删除队头元素。
- 实现 `FrontAndDequeue`：返回队头元素并同时出队。

## 行为要求

- 队列应使用 `Size` 区分空队列和满队列。
- 初始状态应满足 `Size == 0`、`Front == 1`、`Rear == 0`。
- 下标移动必须通过循环方式回到数组开头。
- 满队列继续入队、空队列取队头或出队时应调用 `Error`。
- 入队和出队顺序必须满足先进先出。

## 验证方式

```sh
cc -std=gnu89 queue.c testque.c -o /tmp/testque
/tmp/testque
```
