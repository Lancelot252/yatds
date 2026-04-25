
# 线性表（Linear List）教学内容 —— C 语言版

---

## 一、教学目标

| 维度 | 目标 |
|------|------|
| **知识目标** | 理解线性表的逻辑结构；掌握顺序表与链表两种存储方式；熟悉基本运算的 C 语言实现 |
| **能力目标** | 能用 C 语言指针、结构体、动态内存分配（`malloc`/`free`）实现线性表 |
| **素养目标** | 培养指针思维、内存管理意识和算法设计能力 |
| **课时建议** | 共 8 课时（理论 5 + 实验 3） |

---

## 二、线性表的基本概念

### 1. 定义
线性表是由 **n（n≥0）个具有相同数据类型的数据元素**构成的有限序列：

$$
L = (a_1, a_2, \dots, a_n)
$$

### 2. 逻辑特征（"一对一"关系）
- 存在唯一的"第一个"元素（无前驱）
- 存在唯一的"最后一个"元素（无后继）
- 其余元素都有且仅有一个直接前驱和一个直接后继

### 3. 抽象数据类型 ADT 定义

```
ADT List {
    数据对象：D = {a_i | a_i ∈ ElemType, i = 1, 2, ..., n, n ≥ 0}
    数据关系：R = {<a_(i-1), a_i> | a_(i-1), a_i ∈ D, i = 2, ..., n}
    基本操作：
        InitList(L)         初始化
        DestroyList(L)      销毁
        ListLength(L)       求表长
        GetElem(L, i, e)    取第 i 个元素
        LocateElem(L, e)    按值查找
        ListInsert(L, i, e) 插入
        ListDelete(L, i, e) 删除
        ListEmpty(L)        判空
        ListTraverse(L)     遍历
}
```

---

## 三、线性表的顺序存储结构（顺序表）

### 1. 存储原理
用一组**地址连续**的存储单元依次存储线性表的数据元素。  
逻辑上相邻 ⇔ 物理上相邻。

地址计算公式：
$$
\text{LOC}(a_i) = \text{LOC}(a_1) + (i-1) \times L
$$

### 2. C 语言结构体定义

#### 静态分配版本
```c
#include <stdio.h>
#define MAXSIZE 100
typedef int ElemType;     /* 数据元素类型，可改为 char、float 等 */

typedef struct {
    ElemType data[MAXSIZE];   /* 存储数组 */
    int length;               /* 当前长度 */
} SqList;
```

#### 动态分配版本
```c
#include <stdlib.h>
#define INIT_SIZE 100
typedef int ElemType;

typedef struct {
    ElemType *data;       /* 指向动态分配的数组 */
    int length;           /* 当前长度 */
    int capacity;         /* 当前容量 */
} SqList;
```

### 3. 核心操作（纯 C 实现）

#### （1）初始化
```c
int InitList(SqList *L) {
    L->data = (ElemType *)malloc(INIT_SIZE * sizeof(ElemType));
    if (L->data == NULL) return 0;     /* 分配失败 */
    L->length = 0;
    L->capacity = INIT_SIZE;
    return 1;                          /* 成功返回 1 */
}
```

> 💡 **C 语言要点**：C 没有引用，所以用**指针 `SqList *L`** 来修改结构体。调用时写 `InitList(&L);`

#### （2）插入操作
```c
int ListInsert(SqList *L, int i, ElemType e) {
    int j;
    if (i < 1 || i > L->length + 1) return 0;       /* 位置非法 */
    if (L->length >= L->capacity) return 0;         /* 表已满 */
    for (j = L->length; j >= i; j--)
        L->data[j] = L->data[j - 1];                /* 元素后移 */
    L->data[i - 1] = e;
    L->length++;
    return 1;
}
```
**平均时间复杂度 O(n)**，平均移动次数 = n/2

#### （3）删除操作
```c
int ListDelete(SqList *L, int i, ElemType *e) {
    int j;
    if (i < 1 || i > L->length) return 0;
    *e = L->data[i - 1];                            /* 用指针返回被删元素 */
    for (j = i; j < L->length; j++)
        L->data[j - 1] = L->data[j];                /* 元素前移 */
    L->length--;
    return 1;
}
```
**平均时间复杂度 O(n)**

#### （4）按值查找
```c
int LocateElem(SqList L, ElemType e) {
    int i;
    for (i = 0; i < L.length; i++)
        if (L.data[i] == e) return i + 1;           /* 返回位序，从 1 开始 */
    return 0;                                       /* 未找到 */
}
```

#### （5）遍历输出
```c
void ListTraverse(SqList L) {
    int i;
    for (i = 0; i < L.length; i++)
        printf("%d ", L.data[i]);
    printf("\n");
}
```

#### （6）销毁
```c
void DestroyList(SqList *L) {
    if (L->data != NULL) {
        free(L->data);
        L->data = NULL;
    }
    L->length = 0;
    L->capacity = 0;
}
```

### 4. 顺序表完整测试程序
```c
int main(void) {
    SqList L;
    ElemType e;
    int i;
    
    InitList(&L);
    for (i = 1; i <= 5; i++)
        ListInsert(&L, i, i * 10);          /* 插入 10, 20, 30, 40, 50 */
    printf("当前顺序表：");
    ListTraverse(L);
    
    ListDelete(&L, 3, &e);
    printf("删除第 3 个元素 %d 后：", e);
    ListTraverse(L);
    
    printf("元素 40 的位置：%d\n", LocateElem(L, 40));
    
    DestroyList(&L);
    return 0;
}
```

### 5. 顺序表优缺点
| 优点 | 缺点 |
|------|------|
| 随机存取，O(1) 访问 | 插入删除需移动大量元素 |
| 存储密度高（无指针开销）| 容量固定，扩展困难 |
| 实现简单 | 易造成空间浪费或溢出 |

---

## 四、线性表的链式存储结构（链表）

### 1. 单链表

#### 节点结构定义
```c
typedef int ElemType;

typedef struct LNode {
    ElemType data;             /* 数据域 */
    struct LNode *next;        /* 指针域 */
} LNode, *LinkList;
```

> 💡 **C 语言要点**：`LNode` 是结构体类型名，`LinkList` 是 `LNode *`（指向结点的指针）类型名。两者本质相同，但语义不同：
> - `LNode *p`：强调 p 是一个**结点指针**
> - `LinkList L`：强调 L 是一个**链表**（通常指头指针）

#### 头指针 vs 头结点
- **头指针**：指向链表第一个结点的指针
- **头结点**：在首元结点之前附加的"哨兵"结点（统一空表与非空表的处理逻辑）

#### ① 初始化（带头结点）
```c
int InitList(LinkList *L) {
    *L = (LNode *)malloc(sizeof(LNode));
    if (*L == NULL) return 0;
    (*L)->next = NULL;
    return 1;
}
```

#### ② 头插法建立链表（结果逆序）
```c
void CreateList_Head(LinkList L, int n) {
    int i;
    LNode *p;
    for (i = 0; i < n; i++) {
        p = (LNode *)malloc(sizeof(LNode));
        scanf("%d", &p->data);
        p->next = L->next;        /* 新结点指向原首元结点 */
        L->next = p;              /* 头结点指向新结点 */
    }
}
```

#### ③ 尾插法建立链表（结果顺序）
```c
void CreateList_Tail(LinkList L, int n) {
    int i;
    LNode *p, *r;
    r = L;                                 /* r 始终指向尾结点 */
    for (i = 0; i < n; i++) {
        p = (LNode *)malloc(sizeof(LNode));
        scanf("%d", &p->data);
        p->next = NULL;
        r->next = p;
        r = p;
    }
}
```

#### ④ 按位查找（取第 i 个结点）
```c
LNode* GetElem(LinkList L, int i) {
    int j = 1;
    LNode *p = L->next;          /* 指向首元结点 */
    if (i == 0) return L;        /* 返回头结点 */
    if (i < 1) return NULL;
    while (p != NULL && j < i) {
        p = p->next;
        j++;
    }
    return p;                    /* 找不到返回 NULL */
}
```

#### ⑤ 插入第 i 个位置
```c
int ListInsert(LinkList L, int i, ElemType e) {
    LNode *p, *s;
    p = GetElem(L, i - 1);       /* 找到第 i-1 个结点 */
    if (p == NULL) return 0;
    s = (LNode *)malloc(sizeof(LNode));
    s->data = e;
    s->next = p->next;
    p->next = s;
    return 1;
}
```
**时间复杂度 O(n)**（主要花在查找上）

#### ⑥ 删除第 i 个结点
```c
int ListDelete(LinkList L, int i, ElemType *e) {
    LNode *p, *q;
    p = GetElem(L, i - 1);
    if (p == NULL || p->next == NULL) return 0;
    q = p->next;
    *e = q->data;
    p->next = q->next;
    free(q);                     /* C 语言必须手动释放内存！*/
    return 1;
}
```

> ⚠️ **C 语言重点提醒**：每次 `malloc` 都要对应一次 `free`，否则会**内存泄漏**！

#### ⑦ 遍历输出
```c
void ListTraverse(LinkList L) {
    LNode *p = L->next;
    while (p != NULL) {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("\n");
}
```

#### ⑧ 销毁链表
```c
void DestroyList(LinkList L) {
    LNode *p = L, *q;
    while (p != NULL) {
        q = p->next;
        free(p);
        p = q;
    }
}
```

### 2. 双向链表

#### 节点定义
```c
typedef struct DNode {
    ElemType data;
    struct DNode *prior;     /* 前驱指针 */
    struct DNode *next;      /* 后继指针 */
} DNode, *DLinkList;
```

#### 插入操作（在 p 之后插入 s）
```c
s->next = p->next;
if (p->next != NULL) p->next->prior = s;
s->prior = p;
p->next = s;
```
**⚠️ 顺序不可错！** 先连后断的原则。

#### 删除结点 p
```c
p->prior->next = p->next;
if (p->next != NULL) p->next->prior = p->prior;
free(p);
```

### 3. 循环链表
- **单循环链表**：尾结点 `next` 指向头结点
- **双循环链表**：首尾相连，便于头尾互访

判空条件变为：`p->next == L`（不再是 `NULL`）

---

## 五、顺序表 vs 链表 对比

| 比较项 | 顺序表（数组）| 链表（指针）|
|--------|--------------|------------|
| **存取方式** | 随机存取 O(1) | 顺序存取 O(n) |
| **插入删除** | O(n)，需移动元素 | O(1)，仅修改指针（已定位时）|
| **空间分配** | 静态/预分配 | 动态分配（`malloc`）|
| **存储密度** | 高（=1）| 低（含指针域）|
| **C 关键技术** | 数组下标、`memmove` | 指针、`malloc/free`、结构体自引用 |
| **适用场景** | 频繁查找、长度稳定 | 频繁插入删除、长度变化大 |

---

## 六、C 语言实现的典型例题

### 例 1：将顺序表 L 中所有元素逆置（O(1) 空间）
```c
void Reverse(SqList *L) {
    int i;
    ElemType t;
    for (i = 0; i < L->length / 2; i++) {
        t = L->data[i];
        L->data[i] = L->data[L->length - 1 - i];
        L->data[L->length - 1 - i] = t;
    }
}
```

### 例 2：删除单链表中所有值为 x 的结点
```c
void DeleteX(LinkList L, ElemType x) {
    LNode *pre = L, *p = L->next, *q;
    while (p != NULL) {
        if (p->data == x) {
            q = p;
            pre->next = p->next;
            p = p->next;
            free(q);             /* 释放被删结点 */
        } else {
            pre = p;
            p = p->next;
        }
    }
}
```

### 例 3：合并两个有序链表（升序，带头结点）
```c
LinkList MergeList(LinkList La, LinkList Lb) {
    LNode *pa = La->next, *pb = Lb->next, *r = La;
    LinkList Lc = La;
    free(Lb);                    /* 不再需要 Lb 的头结点 */
    while (pa != NULL && pb != NULL) {
        if (pa->data <= pb->data) {
            r->next = pa; r = pa; pa = pa->next;
        } else {
            r->next = pb; r = pb; pb = pb->next;
        }
    }
    r->next = (pa != NULL) ? pa : pb;
    return Lc;
}
```

### 例 4：判断单链表是否有环（快慢指针法）
```c
int HasCycle(LinkList L) {
    LNode *slow = L, *fast = L;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return 1;     /* 相遇，有环 */
    }
    return 0;                           /* 走到末尾，无环 */
}
```

---

## 七、C 语言相关的重难点

### 🎯 学生最易出错的几个点

| 易错点 | 错误示例 | 正确做法 |
|--------|---------|---------|
| **忘记 `&` 取地址** | `InitList(L)` | `InitList(&L)` |
| **`->` 与 `.` 混用** | `p.data` | `p->data`（p 是指针）|
| **未初始化指针** | `LNode *p; p->data=1;` | 先 `malloc` 再使用 |
| **野指针** | `free(p)` 后继续用 `p` | `free(p); p = NULL;` |
| **内存泄漏** | 忘记 `free` | 每个 `malloc` 都要配 `free` |
| **越界访问** | `L.data[L.length]` 当成最后一个 | 应是 `L.data[L.length-1]` |
| **空指针解引用** | `p->next` 但 p 是 NULL | 操作前判断 `if (p != NULL)` |

### 🛠 调试技巧
1. **使用 `printf` 打印地址**：`printf("%p\n", p);`
2. **画图法**：让学生画出指针变化过程
3. **使用 GDB / VS 调试器**：单步跟踪指针
4. **使用 Valgrind** 检查内存泄漏（Linux 环境）：
   ```bash
   gcc -g main.c -o main
   valgrind --leak-check=full ./main
   ```

---

## 八、教学建议（课改思路）

1. **情境导入**：用"排队买票""通讯录"类比线性表，激发兴趣
2. **对比教学**：顺序表 vs 链表，体会"时间换空间"思想
3. **画图教学**：链表操作必须**先画图后写代码**，让学生养成习惯
4. **可视化工具**：使用 [Visualgo](https://visualgo.net/) 演示插入删除过程
5. **项目式学习（PBL）**：用线性表实现"学生成绩管理系统"
6. **分层练习**：
   - 基础层：手写顺序表/链表的增删
   - 提高层：链表逆置、合并、判环
   - 拓展层：用链表实现一元多项式加法
7. **思政融入**：通过链表"环环相扣"理解团队协作；通过指针操作体会精准、严谨的工匠精神

---

## 九、课后作业设计

| 题型 | 示例 |
|------|------|
| **概念题** | 简述顺序表和链表的区别，并各列举 2 个适用场景 |
| **改错题** | 给一段有 bug 的链表代码（如野指针、内存泄漏），让学生改正 |
| **算法题** | 编写函数删除有序顺序表中的重复元素（O(n) 复杂度）|
| **综合题** | 用链表实现一元多项式 `P(x) = 3x²+5x+1` 的加法运算 |
| **实验题** | 实现单链表 ADT 全部基本操作并测试，要求**无内存泄漏** |

---

## 十、配套 C 语言完整工程结构（建议）

```
LinearList/
├── include/
│   ├── sqlist.h        /* 顺序表头文件 */
│   └── linklist.h      /* 链表头文件 */
├── src/
│   ├── sqlist.c        /* 顺序表实现 */
│   ├── linklist.c      /* 链表实现 */
│   └── main.c          /* 测试主程序 */
├── Makefile            /* 编译脚本 */
└── README.md           /* 说明文档 */
```

### 简单 Makefile 示例
```makefile
CC = gcc
CFLAGS = -Wall -g -Iinclude

main: src/main.c src/sqlist.c src/linklist.c
	$(CC) $(CFLAGS) -o main src/main.c src/sqlist.c src/linklist.c

clean:
	rm -f main *.o
```

