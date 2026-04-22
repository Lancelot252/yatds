# 线性表（数组和链表）

> *<small>数据结构的世界如同一堵厚实的砖墙。</small>*
> *<small>数组的砖块整齐排列，逐个紧贴。链表的砖块分散各处，连接的藤蔓自由地穿梭于砖缝之间。</small>*

所有依赖代码位于 `List\src` 目录下，涵盖了 C语言 和 C++ 版本，同学们可以根据自己的喜好来选择。

## Baseline 任务 1：链表的基本实现

链表（linked list）是一种线性数据结构，其中的每个元素都是一个节点对象，各个节点通过“引用”相连接。引用记录了下一个节点的内存地址，通过它可以从当前节点访问到下一个节点。

链表的设计使得各个节点可以分散存储在内存各处，它们的内存地址无须连续。

![链表定义与存储方式](../../Source/linked_list_definition_storage.png)

链表的组成单位是节点（node）对象。每个节点都包含两项数据：节点的“值”和指向下一节点的“引用”（指针）。

一般而言，一个常见的 C++ 链表结构体定义如下：

```cpp
/* 链表节点结构体 */
struct ListNode {
    ElementType val;         // 节点值
    ListNode *next;  // 指向下一节点的指针
    ListNode(int x) : val(x), next(nullptr) {}  // 构造函数
};
```
在 C 语言中，链表节点的定义类似：

```c
struct ListNode {  
    ElementType val; // 节点的值
    struct ListNode* next;  // 指向下一个节点的指针
};
```
<small>注：`ElementType` 代表节点中存储的数据类型，实际定义中一般为 `int`、`float`、`char*` 等。</small>

### 初始化链表

建立链表分为两步，第一步是初始化各个节点对象，第二步是构建节点之间的引用关系。初始化完成后，我们就可以从链表的头节点出发，通过引用（指针）指向 `next` 依次访问所有节点。

在C++中，链表的初始化可以通过`new`运算符来完成空间分配：

```cpp
/* 初始化链表 1 -> 3 -> 2 -> 5 -> 4 */
// 初始化各个节点
ListNode* n0 = new ListNode(1);
ListNode* n1 = new ListNode(3);
ListNode* n2 = new ListNode(2);
ListNode* n3 = new ListNode(5);
ListNode* n4 = new ListNode(4);
// 构建节点之间的引用
n0->next = n1;
n1->next = n2;
n2->next = n3;
n3->next = n4;
```

而在C语言中，我们需要使用`malloc`函数来分配内存：

```c
/* 初始化链表 1 -> 3 -> 2 -> 5 -> 4 */
// 初始化各个节点
struct Node* n0 = (struct Node*)malloc(sizeof(struct Node));
n0->Element = 1;
struct Node* n1 = (struct Node*)malloc(sizeof(struct Node));
n1->Element = 3;
struct Node* n2 = (struct Node*)malloc(sizeof(struct Node));
n2->Element = 2;
struct Node* n3 = (struct Node*)malloc(sizeof(struct Node));
n3->Element = 5;
struct Node* n4 = (struct Node*)malloc(sizeof(struct Node));
n4->Element = 4;
// 构建节点之间的引用
n0->Next = n1;
n1->Next = n2;
n2->Next = n3;
n3->Next = n4;
```
<small>注：`malloc` 函数返回一个 `void*` 类型的指针，需要进行类型转换以匹配目标结构体指针类型。`malloc` 位于 `stdlib.h` 头文件中，使用前需要包含该头文件。</small>

数组整体是一个变量，比如数组 nums 包含元素 nums[0] 和 nums[1] 等，而链表是由多个独立的节点对象组成的。我们通常将头节点当作链表的代称，比如以上代码中的链表可记作链表 n0 。

### 插入节点

在链表中插入节点非常容易。如图所示，假设我们想在相邻的两个节点 `n0` 和 `n1` 之间插入一个新节点 `P` ，则只需改变两个节点引用（指针）即可，时间复杂度为 $O(1)$ 。

相比之下，在数组中插入元素的时间复杂度为 $O(n)$ ，在大数据量下的效率较低。

![节点的插入](../../Source/insert_node.png)
C++版本的插入代码如下：

```cpp
/* 在链表的节点 n0 之后插入节点 P */
void insert(ListNode *n0, ListNode *P) {
    ListNode *n1 = n0->next;
    P->next = n1;
    n0->next = P;
}
```
C语言版本的插入代码如下：
```c
/* 在链表的节点 n0 之后插入节点 P */
void insert(ListNode *n0, ListNode *P) {
    ListNode *n1 = n0->next;
    P->next = n1;
    n0->next = P;
}
```
可以看到，实际上C语言和C++的链表插入代码完全相同，唯一的区别在于数据类型的定义和内存分配方式不同。

### 删除节点

如图所示，在链表中删除节点也非常方便，只需改变一个节点的引用（指针）即可。

请注意，尽管在删除操作完成后节点 `P` 仍然指向 `n1` ，但实际上遍历此链表已经无法访问到 `P` ，这意味着 `P` 已经不再属于该链表了。

![节点的删除](../../Source/delete_node.png)
C++版本的删除代码如下：
```cpp
/* 删除链表的节点 n0 之后的首个节点 */
void remove(ListNode *n0) {
    if (n0->next == nullptr)
        return;
    // n0 -> P -> n1
    ListNode *P = n0->next;
    ListNode *n1 = P->next;
    n0->next = n1;
    // 释放内存
    delete P;
}
```
C语言版本的删除代码如下：
```c
/* 删除链表的节点 n0 之后的首个节点 */
// 注意：stdio.h 占用了 remove 关键词
void removeItem(ListNode *n0) {
    if (!n0->next)
        return;
    // n0 -> P -> n1
    ListNode *P = n0->next;
    ListNode *n1 = P->next;
    n0->next = n1;
    // 释放内存
    free(P);
}
```

<small>注：内存释放是必要的，否则会造成程序内存溢出。</small>

### 访问节点

**在链表中访问节点的效率较低。** 如之前所述，我们可以在 $O(1)$ 时间下访问数组中的任意元素。链表则不然，程序需要从头节点出发，逐个向后遍历，直至找到目标节点。也就是说，访问链表的第 $i$ 个节点需要循环 $i - 1$ 轮，时间复杂度为 $O(n)$ 。

代码如下所示（C++和C语言版本相同）：

```cpp
/* 访问链表中索引为 index 的节点 */
ListNode *access(ListNode *head, int index) {
    for (int i = 0; i < index; i++) {
        if (head == nullptr)
            return nullptr;
        head = head->next;
    }
    return head;
}
```

### 查找节点

遍历链表，查找其中值为 `target` 的节点，输出该节点在链表中的索引。此过程也属于线性查找。

代码如下所示（C++和C语言版本相同）：

```cpp
/* 在链表中查找值为 target 的首个节点 */
int find(ListNode *head, int target) {
    int index = 0;
    while (head != nullptr) {
        if (head->val == target)
            return index;
        head = head->next;
        index++;
    }
    return -1;
}
```

### 任务内容

我们需要同学们完成以下内容：

- 编写链表的基本实现代码，包括节点定义、链表初始化、节点插入、节点删除、节点访问和节点查找等功能（希望同学们可以超出上述教程中给出的几种操作，编写出更多的功能函数），并写出一个测试程序来验证这些功能的正确性。（注：测试程序需要能够体现函数的特性，不能仅仅是简单的功能测试。）

- 尝试将链表实现为一个模版类（C++）或者一个通用的链表实现（C），支持任意数据类型的存储。编写你的测试程序来验证（要求与前一个任务相同）。

- 我们在仓库中提供了一个测试程序 `testlist.cpp`和 `testlist.c`，同学们需要根据这个测试程序来完成链表的基本实现，并在完成后加入在前两个任务中实现的功能。加入新功能后的链表实现需要能够同时正确执行我们给出的测试程序和你自己编写的新测试程序。

你提交的材料中这部分内容需要包含：
- 三个小任务分别的代码（注意：代码的文件结构和注释也是评分的一部分）
- 对你程序和结果的分析（注：分析需要能够体现你对这些功能的理解，不能仅仅是简单的结果描述）
