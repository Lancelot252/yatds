# 线性表（数组和链表）

> *<small>数据结构的世界如同一堵厚实的砖墙。</small>*
> *<small>数组的砖块整齐排列，逐个紧贴。链表的砖块分散各处，连接的藤蔓自由地穿梭于砖缝之间。</small>*

所有依赖代码位于 `List\src` 目录下，涵盖了 C语言 和 C++ 版本，同学们可以根据自己的喜好来选择。

## Baseline 任务 1：链表的基本实现

链表（linked list）是一种线性数据结构，其中的每个元素都是一个节点对象，各个节点通过“引用”相连接。引用记录了下一个节点的内存地址，通过它可以从当前节点访问到下一个节点。

链表的设计使得各个节点可以分散存储在内存各处，它们的内存地址无须连续。

下面的 HTML 示意图分步展示了链表节点的定义、逻辑连接关系，以及链表节点在内存中可以分散存储的特点。

[打开链表定义与存储方式示意图](../../Source/linked_list_definition_storage.html)

<iframe src="../../Source/linked_list_definition_storage.html" width="100%" height="860" style="border: 1px solid #d8ded9; border-radius: 12px;"></iframe>

