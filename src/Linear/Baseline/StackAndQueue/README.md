# 栈和队列
> *<small>栈如同叠猫猫，而队列就像猫猫排队。</small>*
> *<small>两者分别代表先入后出和先入先出的逻辑关系。</small>*

所有依赖代码位于 `StackAndQueue\src` 目录下。

## Baseline 任务 1: 栈的基本实现

栈（stack）是一种遵循先入后出逻辑的线性数据结构。

我们可以将栈类比为桌面上的一摞盘子，规定每次只能移动一个盘子，那么想取出底部的盘子，则需要先将上面的盘子依次移走。我们将盘子替换为各种类型的元素（如整数、字符、对象等），就得到了栈这种数据结构。

如图所示，我们把堆叠元素的顶部称为“栈顶”，底部称为“栈底”。将把元素添加到栈顶的操作叫作“入栈”，删除栈顶元素的操作叫作“出栈”。
![stack](../../Source/stack.png)

### 栈的基本操作
栈的常用操作如表所示，具体的方法名需要根据所使用的编程语言来确定。在此，我们以常见的 push()、pop()、top() 命名为例（即 C++ 中`<stack>`库中栈的对应方法）。

| 方法 | 描述 | 时间复杂度 |
| --- | --- | --- |
| `push()` | 元素入栈（添加至栈顶） | $O(1)$ |
| `pop()` | 栈顶元素出栈 | $O(1)$ |
| `top()` | 访问栈顶元素 | $O(1)$ |


没错，在 C++ 语言中，栈的数据结构已经被封装在 `<stack>` 库中，我们可以直接使用它来创建和操作栈。以下是一个简单的示例：

```cpp
/* 初始化栈 */
stack<int> stack;

/* 元素入栈 */
stack.push(1);
stack.push(3);
stack.push(2);
stack.push(5);
stack.push(4);

/* 访问栈顶元素 */
int top = stack.top();

/* 元素出栈 */
stack.pop(); // 无返回值

/* 获取栈的长度 */
int size = stack.size();

/* 判断是否为空 */
bool empty = stack.empty();
```
<small>注：上述代码需要包含头文件 `#include <stack>`</small>

当然，实际上`<stack>`库中的栈包含的函数远超过上述三种基本操作，大家可以参考 C++ 官方文档来了解更多细节：https://en.cppreference.com/w/cpp/container/stack 。

### 栈的实现
为了深入了解栈的运行机制，我们来尝试自己实现一个栈类。

栈遵循先入后出的原则，因此我们只能在栈顶添加或删除元素。然而，数组和链表都可以在任意位置添加和删除元素，**因此栈可以视为一种受限制的数组或链表**。换句话说，我们可以“屏蔽”数组或链表的部分无关操作，使其对外表现的逻辑符合栈的特性。

#### 1.基于链表的栈实现
使用链表实现栈时，我们可以将链表的头节点视为栈顶，尾节点视为栈底。

如图所示，对于入栈操作，我们只需将元素插入链表头部，这种节点插入方法被称为“头插法”。而对于出栈操作，只需将头节点从链表中删除即可。
![stack_linked_list_1](../../Source/stack_linked_list1.png)

![stack_linked_list_2](../../Source/stack_linked_list2.png)

![stack_linked_list_3](../../Source/stack_linked_list3.png)

以下是基于链表实现栈的示例代码：

```cpp
/* 基于链表实现的栈 */
class LinkedListStack {
  private:
    ListNode *stackTop; // 将头节点作为栈顶
    int stkSize;        // 栈的长度

  public:
    LinkedListStack() {
        stackTop = nullptr;
        stkSize = 0;
    }

    ~LinkedListStack() {
        // 遍历链表删除节点，释放内存
        freeMemoryLinkedList(stackTop);
    }

    /* 获取栈的长度 */
    int size() {
        return stkSize;
    }

    /* 判断栈是否为空 */
    bool isEmpty() {
        return size() == 0;
    }

    /* 入栈 */
    void push(int num) {
        ListNode *node = new ListNode(num);
        node->next = stackTop;
        stackTop = node;
        stkSize++;
    }

    /* 出栈 */
    int pop() {
        int num = top();
        ListNode *tmp = stackTop;
        stackTop = stackTop->next;
        // 释放内存
        delete tmp;
        stkSize--;
        return num;
    }

    /* 访问栈顶元素 */
    int top() {
        if (isEmpty())
            throw out_of_range("栈为空");
        return stackTop->val;
    }
};
```
<small>注：上述代码中的 `ListNode` 是链表节点的定义。</small>

#### 2.基于数组的栈实现

使用数组实现栈时，我们可以将数组的尾部作为栈顶。如图所示，入栈与出栈操作分别对应在数组尾部添加元素与删除元素，时间复杂度都为 $O(1)$。
![stack_array_1](../../Source/stack_array1.png)

![stack_array_2](../../Source/stack_array2.png)

![stack_array_3](../../Source/stack_array3.png)

```cpp
/* 基于数组实现的栈 */
class ArrayStack {
  private:
    vector<int> stack;

  public:
    /* 获取栈的长度 */
    int size() {
        return stack.size();
    }

    /* 判断栈是否为空 */
    bool isEmpty() {
        return stack.size() == 0;
    }

    /* 入栈 */
    void push(int num) {
        stack.push_back(num);
    }

    /* 出栈 */
    int pop() {
        int num = top();
        stack.pop_back();
        return num;
    }

    /* 访问栈顶元素 */
    int top() {
        if (isEmpty())
            throw out_of_range("栈为空");
        return stack.back();
    }

    /* 返回 Vector */
    vector<int> toVector() {
        return stack;
    }
};
```
<small>注：上述代码中的 `vector` 是 C++ 标准库中的动态数组。</small>

