
# 线性表（Linear List）教学内容


---

## 一、教学目标

| 维度 | 目标 |
|------|------|
| **知识目标** | 理解线性表的逻辑结构；掌握顺序表与链表两种存储方式；熟悉基本运算的实现 |
| **能力目标** | 能根据实际问题选择合适的存储结构；能用代码实现增删改查算法 |
| **素养目标** | 培养抽象思维、算法设计能力和工程化思想 |
| **课时建议** | 共 8 课时（理论 5 + 实验 3） |

---

## 二、线性表的基本概念

### 1. 定义
线性表（Linear List）是由 **n（n≥0）个具有相同数据类型的数据元素** 构成的有限序列，记作：

$$
L = (a_1, a_2, a_3, \dots, a_{i-1}, a_i, a_{i+1}, \dots, a_n)
$$

- $a_1$：表头元素（无前驱）
- $a_n$：表尾元素（无后继）
- $a_i$：第 i 个元素，称为数据元素
- **n** 为表长，n=0 时为空表

### 2. 逻辑特征（"一对一"关系）
- 存在唯一的"第一个"元素
- 存在唯一的"最后一个"元素
- 除第一个外，每个元素有且仅有一个**直接前驱**
- 除最后一个外，每个元素有且仅有一个**直接后继**

### 3. 抽象数据类型 ADT 定义

```
ADT List {
    数据对象：D = {a_i | a_i ∈ ElemType, i = 1, 2, ..., n, n ≥ 0}
    数据关系：R = {<a_(i-1), a_i> | a_(i-1), a_i ∈ D, i = 2, ..., n}
    基本操作：
        InitList(&L)        // 初始化
        DestroyList(&L)     // 销毁
        ListLength(L)       // 求表长
        GetElem(L, i, &e)   // 取第i个元素
        LocateElem(L, e)    // 按值查找
        ListInsert(&L, i, e)// 插入
        ListDelete(&L, i, &e)// 删除
        ListEmpty(L)        // 判空
        ListTraverse(L)     // 遍历
}
```

---

## 三、线性表的顺序存储结构（顺序表）

### 1. 存储原理
用一组**地址连续**的存储单元依次存储线性表的数据元素，逻辑上相邻 ⇔ 物理上相邻。

存储地址公式：
$$
\text{LOC}(a_i) = \text{LOC}(a_1) + (i-1) \times L
$$
其中 L 为每个元素所占字节数。

### 2. C 语言定义

```c
#define MAXSIZE 100
typedef struct {
    ElemType data[MAXSIZE];   // 静态数组
    int length;               // 当前长度
} SqList;
```

动态分配版本：
```c
typedef struct {
    ElemType *data;
    int length;
    int capacity;
} SqList;
```

### 3. 核心操作及时间复杂度

#### （1）插入操作

```c
bool ListInsert(SqList &L, int i, ElemType e) {
    if (i < 1 || i > L.length + 1) return false;
    if (L.length >= MAXSIZE) return false;
    for (int j = L.length; j >= i; j--)
        L.data[j] = L.data[j-1];     // 元素后移
    L.data[i-1] = e;
    L.length++;
    return true;
}
```
**平均时间复杂度 O(n)**，平均移动次数 = n/2

#### （2）删除操作
```c
bool ListDelete(SqList &L, int i, ElemType &e) {
    if (i < 1 || i > L.length) return false;
    e = L.data[i-1];
    for (int j = i; j < L.length; j++)
        L.data[j-1] = L.data[j];     // 元素前移
    L.length--;
    return true;
}
```
**平均时间复杂度 O(n)**，平均移动次数 = (n-1)/2

#### （3）按值查找
```c
int LocateElem(SqList L, ElemType e) {
    for (int i = 0; i < L.length; i++)
        if (L.data[i] == e) return i + 1;
    return 0;
}
```
**O(n)**

### 4. 顺序表优缺点
| 优点 | 缺点 |
|------|------|
| 随机存取，O(1) 访问 | 插入删除需移动大量元素 |
| 存储密度高（无指针开销） | 容量固定，扩展困难 |
| 实现简单 | 易造成存储空间浪费或溢出 |

---

## 四、线性表的链式存储结构（链表）

### 1. 单链表

#### 节点结构
```c
typedef struct LNode {
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;
```

#### 头指针 vs 头结点
- **头指针**：指向链表第一个结点的指针
- **头结点**：在首元结点之前附加的结点（统一空表与非空表的处理）

#### 关键操作

**① 头插法建立链表**（逆序）
```c
void CreateList_H(LinkList &L, int n) {
    L = (LNode*)malloc(sizeof(LNode));
    L->next = NULL;
    for (int i = 0; i < n; i++) {
        LNode *p = (LNode*)malloc(sizeof(LNode));
        scanf("%d", &p->data);
        p->next = L->next;
        L->next = p;
    }
}
```

**② 尾插法建立链表**（顺序）
```c
void CreateList_T(LinkList &L, int n) {
    L = (LNode*)malloc(sizeof(LNode));
    L->next = NULL;
    LNode *r = L;
    for (int i = 0; i < n; i++) {
        LNode *p = (LNode*)malloc(sizeof(LNode));
        scanf("%d", &p->data);
        p->next = NULL;
        r->next = p;
        r = p;
    }
}
```

**③ 插入（在第 i 位置插入 e）**
```c
bool ListInsert(LinkList &L, int i, ElemType e) {
    LNode *p = L; int j = 0;
    while (p && j < i-1) { p = p->next; j++; }
    if (!p || j > i-1) return false;
    LNode *s = (LNode*)malloc(sizeof(LNode));
    s->data = e;
    s->next = p->next;
    p->next = s;
    return true;
}
```

**④ 删除第 i 个结点**
```c
bool ListDelete(LinkList &L, int i, ElemType &e) {
    LNode *p = L; int j = 0;
    while (p->next && j < i-1) { p = p->next; j++; }
    if (!p->next || j > i-1) return false;
    LNode *q = p->next;
    e = q->data;
    p->next = q->next;
    free(q);
    return true;
}
```

### 2. 双向链表

```c
typedef struct DNode {
    ElemType data;
    struct DNode *prior, *next;
} DNode, *DLinkList;
```

**插入操作（在 p 之后插入 s）**：
```c
s->next = p->next;
p->next->prior = s;
s->prior = p;
p->next = s;
```
顺序不可错！

### 3. 循环链表
- **单循环链表**：尾结点 next 指向头结点
- **双循环链表**：首尾相连，便于头尾互访

---

## 五、顺序表 vs 链表 对比

| 比较项 | 顺序表 | 链表 |
|--------|--------|------|
| **存取方式** | 随机存取 O(1) | 顺序存取 O(n) |
| **插入删除** | O(n)，需移动元素 | O(1)，仅修改指针（已定位时）|
| **空间分配** | 静态/预分配 | 动态分配 |
| **存储密度** | 高（=1） | 低（含指针域） |
| **适用场景** | 频繁查找、长度稳定 | 频繁插入删除、长度变化大 |

---

## 六、典型例题（建议课堂精讲）

**例 1**：将顺序表 L 中所有元素逆置，要求空间复杂度 O(1)。

```c
void Reverse(SqList &L) {
    for (int i = 0; i < L.length/2; i++) {
        ElemType t = L.data[i];
        L.data[i] = L.data[L.length-1-i];
        L.data[L.length-1-i] = t;
    }
}
```

**例 2**：删除单链表中所有值为 x 的结点。

```c
void DeleteX(LinkList &L, ElemType x) {
    LNode *p = L->next, *pre = L, *q;
    while (p) {
        if (p->data == x) {
            q = p; pre->next = p->next; p = p->next; free(q);
        } else { pre = p; p = p->next; }
    }
}
```

**例 3**：合并两个有序链表为一个有序链表（升序）。

**例 4**：判断单链表是否有环（Floyd 快慢指针法）。

---

## 七、教学建议

1. **情境导入**：用"排队买票""通讯录"类比线性表，激发兴趣
2. **对比教学**：顺序表与链表对比讲解，体会"时间换空间"思想
3. **可视化工具**：推荐使用 [Visualgo](https://visualgo.net/) 演示插入删除过程
4. **项目式学习（PBL）**：让学生用线性表实现"学生成绩管理系统"
5. **分层练习**：
   - 基础层：手写顺序表/链表的增删
   - 提高层：链表逆置、合并、判环
   - 拓展层：跳表、LRU 缓存（双向链表+哈希）
6. **思政融入**：通过链表"环环相扣"理解团队协作；通过算法优化体会精益求精的工匠精神

---

## 八、课后作业设计

| 题型 | 示例 |
|------|------|
| 概念题 | 简述顺序表和链表的区别 |
| 算法题 | 编写函数删除有序顺序表中的重复元素 |
| 综合题 | 设计一个一元多项式加法运算（用链表实现）|
| 实验题 | 实现单链表 ADT 全部操作并测试 |

---

