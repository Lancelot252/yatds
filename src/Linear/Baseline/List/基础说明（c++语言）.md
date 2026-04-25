# 线性表（Linear List）教学内容 —— C++ 版

下面是基于 **现代 C++（C++11/14/17）** 重新编写的"线性表"完整教学内容。代码采用**面向对象（OOP）+ 模板（Template）+ 异常处理**的现代 C++ 风格，可在 `g++`、`clang++`、`Visual Studio` 中编译运行。

---

## 一、教学目标

| 维度 | 目标 |
|------|------|
| **知识目标** | 理解线性表的逻辑结构；掌握用类（class）封装顺序表与链表；理解模板编程 |
| **能力目标** | 能用 C++ 类、模板、引用、动态内存管理实现线性表；理解 RAII 思想 |
| **素养目标** | 培养面向对象思维、泛型编程能力和资源管理意识 |
| **课时建议** | 共 8 课时（理论 5 + 实验 3） |

---

## 二、线性表的基本概念

### 1. 定义
线性表是由 **n（n≥0）个具有相同数据类型的数据元素**构成的有限序列：

$$
L = (a_1, a_2, \dots, a_n)
$$

### 2. 逻辑特征
- 唯一的"第一个"元素（无前驱）
- 唯一的"最后一个"元素（无后继）
- 中间元素都有且仅有一个直接前驱和一个直接后继

### 3. C++ 抽象数据类型（用类来表达 ADT）

```cpp
template <typename T>
class List {
public:
    virtual ~List() = default;                       // 虚析构函数
    virtual bool empty() const = 0;                  // 判空
    virtual int size() const = 0;                    // 求长度
    virtual T get(int i) const = 0;                  // 取第 i 个元素
    virtual int locate(const T& e) const = 0;        // 按值查找
    virtual void insert(int i, const T& e) = 0;     // 插入
    virtual void remove(int i) = 0;                  // 删除
    virtual void traverse() const = 0;               // 遍历
};
```

> 💡 **C++ 要点**：通过**抽象基类**定义统一接口，顺序表和链表都继承它，体现**多态**。

---

## 三、线性表的顺序存储结构（顺序表）

### 1. 类的定义（使用模板）

```cpp
#include <iostream>
#include <stdexcept>

template <typename T>
class SqList : public List<T> {
private:
    T* data;            // 指向动态数组的指针
    int length;         // 当前长度
    int capacity;       // 容量

    void expand();      // 私有方法：扩容

public:
    // 构造函数
    explicit SqList(int cap = 100);
    
    // 拷贝构造函数（深拷贝）
    SqList(const SqList& other);
    
    // 移动构造函数（C++11）
    SqList(SqList&& other) noexcept;
    
    // 拷贝赋值运算符
    SqList& operator=(const SqList& other);
    
    // 移动赋值运算符
    SqList& operator=(SqList&& other) noexcept;
    
    // 析构函数（RAII 自动释放）
    ~SqList() override;
    
    // 接口实现
    bool empty() const override { return length == 0; }
    int size() const override { return length; }
    T get(int i) const override;
    int locate(const T& e) const override;
    void insert(int i, const T& e) override;
    void remove(int i) override;
    void traverse() const override;
    
    // 运算符重载
    T& operator[](int i);
    const T& operator[](int i) const;
};
```

### 2. 核心操作的 C++ 实现

#### （1）构造函数
```cpp
template <typename T>
SqList<T>::SqList(int cap) : length(0), capacity(cap) {
    data = new T[capacity];        // C++ 用 new 而非 malloc
}
```

#### （2）析构函数（RAII 思想）
```cpp
template <typename T>
SqList<T>::~SqList() {
    delete[] data;                 // 自动释放内存
}
```

> 💡 **C++ 核心思想**：**RAII（Resource Acquisition Is Initialization）**——构造时获取资源，析构时自动释放，避免手动 free 的麻烦。

#### （3）拷贝构造（深拷贝）
```cpp
template <typename T>
SqList<T>::SqList(const SqList& other) 
    : length(other.length), capacity(other.capacity) {
    data = new T[capacity];
    for (int i = 0; i < length; i++)
        data[i] = other.data[i];
}
```

#### （4）移动构造（C++11，避免不必要拷贝）
```cpp
template <typename T>
SqList<T>::SqList(SqList&& other) noexcept 
    : data(other.data), length(other.length), capacity(other.capacity) {
    other.data = nullptr;          // 转移所有权
    other.length = 0;
    other.capacity = 0;
}
```

#### （5）插入操作
```cpp
template <typename T>
void SqList<T>::insert(int i, const T& e) {
    if (i < 1 || i > length + 1)
        throw std::out_of_range("插入位置非法");
    if (length >= capacity)
        expand();                  // 自动扩容
    for (int j = length; j >= i; j--)
        data[j] = data[j - 1];     // 元素后移
    data[i - 1] = e;
    length++;
}
```

#### （6）扩容（动态数组特性）
```cpp
template <typename T>
void SqList<T>::expand() {
    capacity *= 2;
    T* newData = new T[capacity];
    for (int i = 0; i < length; i++)
        newData[i] = data[i];
    delete[] data;
    data = newData;
}
```

#### （7）删除操作
```cpp
template <typename T>
void SqList<T>::remove(int i) {
    if (i < 1 || i > length)
        throw std::out_of_range("删除位置非法");
    for (int j = i; j < length; j++)
        data[j - 1] = data[j];     // 元素前移
    length--;
}
```

#### （8）按位查找（异常处理）
```cpp
template <typename T>
T SqList<T>::get(int i) const {
    if (i < 1 || i > length)
        throw std::out_of_range("访问越界");
    return data[i - 1];
}
```

#### （9）按值查找
```cpp
template <typename T>
int SqList<T>::locate(const T& e) const {
    for (int i = 0; i < length; i++)
        if (data[i] == e) return i + 1;
    return 0;
}
```

#### （10）遍历输出
```cpp
template <typename T>
void SqList<T>::traverse() const {
    for (int i = 0; i < length; i++)
        std::cout << data[i] << " ";
    std::cout << std::endl;
}
```

#### （11）下标运算符重载（让顺序表"像数组"）
```cpp
template <typename T>
T& SqList<T>::operator[](int i) {
    if (i < 0 || i >= length)
        throw std::out_of_range("下标越界");
    return data[i];                // 返回引用，可读可写
}
```

### 3. 顺序表测试程序

```cpp
int main() {
    try {
        SqList<int> L(10);
        for (int i = 1; i <= 5; i++)
            L.insert(i, i * 10);
        
        std::cout << "当前顺序表：";
        L.traverse();              // 10 20 30 40 50
        
        L.remove(3);
        std::cout << "删除第 3 个元素后：";
        L.traverse();              // 10 20 40 50
        
        std::cout << "L[1] = " << L[1] << std::endl;
        L[1] = 99;                 // 通过下标修改
        L.traverse();              // 10 99 40 50
        
        // 测试拷贝构造
        SqList<int> L2 = L;
        std::cout << "L2 内容：";
        L2.traverse();
    } catch (const std::exception& e) {
        std::cerr << "错误：" << e.what() << std::endl;
    }
    return 0;
}
```

### 4. 顺序表优缺点
| 优点 | 缺点 |
|------|------|
| 随机存取 O(1) | 插入删除需移动元素 |
| 缓存友好（连续内存）| 容量需扩展 |
| 配合模板可用于任意类型 | 大量元素时扩容代价高 |

---

## 四、线性表的链式存储结构（链表）

### 1. 单链表类设计

#### 节点类
```cpp
template <typename T>
struct Node {
    T data;
    Node<T>* next;
    
    Node() : next(nullptr) {}
    explicit Node(const T& e, Node<T>* n = nullptr) : data(e), next(n) {}
};
```

> 💡 C++11 后推荐使用 `nullptr` 替代 `NULL`，类型更安全。

#### 链表类
```cpp
template <typename T>
class LinkList : public List<T> {
private:
    Node<T>* head;            // 头结点指针
    int length;

public:
    LinkList();
    LinkList(const LinkList& other);             // 深拷贝
    LinkList(LinkList&& other) noexcept;         // 移动构造
    LinkList& operator=(const LinkList& other);
    LinkList& operator=(LinkList&& other) noexcept;
    ~LinkList() override;
    
    bool empty() const override { return length == 0; }
    int size() const override { return length; }
    T get(int i) const override;
    int locate(const T& e) const override;
    void insert(int i, const T& e) override;
    void remove(int i) override;
    void traverse() const override;
    
    // 特色操作
    void reverse();                              // 链表逆置
    void createHead(const T arr[], int n);       // 头插法建立
    void createTail(const T arr[], int n);       // 尾插法建立
};
```

### 2. 核心操作实现

#### ① 构造函数（建立头结点）
```cpp
template <typename T>
LinkList<T>::LinkList() : length(0) {
    head = new Node<T>();         // 头结点
}
```

#### ② 析构函数（释放所有结点）
```cpp
template <typename T>
LinkList<T>::~LinkList() {
    Node<T>* p = head;
    while (p != nullptr) {
        Node<T>* q = p->next;
        delete p;
        p = q;
    }
}
```

#### ③ 头插法
```cpp
template <typename T>
void LinkList<T>::createHead(const T arr[], int n) {
    for (int i = 0; i < n; i++) {
        Node<T>* p = new Node<T>(arr[i]);
        p->next = head->next;
        head->next = p;
        length++;
    }
}
```

#### ④ 尾插法
```cpp
template <typename T>
void LinkList<T>::createTail(const T arr[], int n) {
    Node<T>* r = head;
    while (r->next != nullptr) r = r->next;   // 找到尾结点
    for (int i = 0; i < n; i++) {
        Node<T>* p = new Node<T>(arr[i]);
        r->next = p;
        r = p;
        length++;
    }
}
```

#### ⑤ 插入第 i 个位置
```cpp
template <typename T>
void LinkList<T>::insert(int i, const T& e) {
    if (i < 1 || i > length + 1)
        throw std::out_of_range("插入位置非法");
    Node<T>* p = head;
    for (int j = 1; j < i; j++) p = p->next;
    Node<T>* s = new Node<T>(e, p->next);
    p->next = s;
    length++;
}
```

#### ⑥ 删除第 i 个结点
```cpp
template <typename T>
void LinkList<T>::remove(int i) {
    if (i < 1 || i > length)
        throw std::out_of_range("删除位置非法");
    Node<T>* p = head;
    for (int j = 1; j < i; j++) p = p->next;
    Node<T>* q = p->next;
    p->next = q->next;
    delete q;                       // C++ 用 delete
    length--;
}
```

#### ⑦ 链表逆置（经典面试题）
```cpp
template <typename T>
void LinkList<T>::reverse() {
    Node<T>* prev = nullptr;
    Node<T>* curr = head->next;
    head->next = nullptr;
    while (curr != nullptr) {
        Node<T>* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    head->next = prev;
}
```

#### ⑧ 遍历
```cpp
template <typename T>
void LinkList<T>::traverse() const {
    Node<T>* p = head->next;
    while (p != nullptr) {
        std::cout << p->data << " ";
        p = p->next;
    }
    std::cout << std::endl;
}
```

### 3. 链表测试程序

```cpp
int main() {
    LinkList<int> L;
    int arr[] = {10, 20, 30, 40, 50};
    L.createTail(arr, 5);
    
    std::cout << "原链表：";
    L.traverse();                     // 10 20 30 40 50
    
    L.insert(3, 99);
    std::cout << "插入后：";
    L.traverse();                     // 10 20 99 30 40 50
    
    L.remove(1);
    std::cout << "删除后：";
    L.traverse();                     // 20 99 30 40 50
    
    L.reverse();
    std::cout << "逆置后：";
    L.traverse();                     // 50 40 30 99 20
    
    return 0;
}
```

### 4. 双向链表（C++ 类设计）

```cpp
template <typename T>
struct DNode {
    T data;
    DNode<T>* prior;
    DNode<T>* next;
    
    explicit DNode(const T& e = T()) 
        : data(e), prior(nullptr), next(nullptr) {}
};
```

**插入操作（在 p 之后插入 e）**
```cpp
DNode<T>* s = new DNode<T>(e);
s->next = p->next;
if (p->next != nullptr) p->next->prior = s;
s->prior = p;
p->next = s;
```

**删除操作**
```cpp
p->prior->next = p->next;
if (p->next != nullptr) p->next->prior = p->prior;
delete p;
```

---

## 五、顺序表 vs 链表 对比（C++ 视角）

| 比较项 | 顺序表 | 链表 |
|--------|--------|------|
| **存取方式** | 随机存取 O(1) | 顺序存取 O(n) |
| **插入删除** | O(n) | O(1)（已定位时）|
| **空间分配** | 连续内存（缓存友好）| 离散内存 |
| **C++ 实现要点** | 模板类 + 动态数组 + 拷贝控制 | 模板类 + 指针 + 节点结构 |
| **STL 对应** | `std::vector` | `std::list` / `std::forward_list` |

> 💡 **C++ 标准库已提供工业级实现**：教学时可以让学生先自己实现，再对比 STL 学习其设计思想。

---

## 六、C++ 典型例题

### 例 1：使用 STL 容器对比实现
```cpp
#include <vector>
#include <list>
#include <iostream>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::list<int> l = {1, 2, 3, 4, 5};
    
    v.insert(v.begin() + 2, 99);     // O(n)
    l.insert(std::next(l.begin(), 2), 99);  // O(1)（定位后）
    
    for (int x : v) std::cout << x << " ";   // 1 2 99 3 4 5
    std::cout << std::endl;
    return 0;
}
```

### 例 2：合并两个有序链表
```cpp
template <typename T>
LinkList<T> merge(LinkList<T>& La, LinkList<T>& Lb) {
    LinkList<T> Lc;
    // ... 实现归并逻辑
    return Lc;                       // 利用移动语义自动优化
}
```

### 例 3：判断链表是否有环（快慢指针）
```cpp
template <typename T>
bool hasCycle(Node<T>* head) {
    Node<T>* slow = head;
    Node<T>* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}
```

### 例 4：使用智能指针（现代 C++ 进阶）
```cpp
#include <memory>

template <typename T>
struct SmartNode {
    T data;
    std::shared_ptr<SmartNode<T>> next;     // 自动管理内存
    explicit SmartNode(const T& e) : data(e), next(nullptr) {}
};
```

> 💡 用 `shared_ptr` / `unique_ptr` 可以**完全免除 delete**，进一步贯彻 RAII 思想。

---

## 七、C++ 重难点提示

### 🎯 学生最易出错的几个点

| 易错点 | 说明 | 解决 |
|--------|------|------|
| **浅拷贝陷阱** | 默认拷贝构造只复制指针 | 必须自定义拷贝构造、赋值运算符 |
| **三/五法则** | 有自定义析构就要写拷贝/移动 | 遵循 Rule of Three / Five |
| **new/delete 不配对** | 内存泄漏 | 优先使用智能指针 |
| **模板编译错误** | 提示信息冗长难懂 | 教学时先用具体类型，再泛化 |
| **虚析构缺失** | 基类指针 delete 派生类对象 | 基类析构必须 `virtual` |
| **`->` vs `.`** | 指针用 `->`，对象用 `.` | 反复练习 |
| **内存泄漏** | `new` 后忘记 `delete` | 使用 `valgrind` 或 智能指针 |

### 🛠 现代 C++ 特性建议

| 特性 | 教学建议 |
|------|---------|
| `nullptr` | 替代 NULL，全程使用 |
| `auto` | 简化迭代器声明 |
| 范围 for | `for (auto& x : container)` |
| `override` | 重写虚函数时写明 |
| `noexcept` | 移动构造/赋值标记 |
| `std::unique_ptr` | 进阶班讲解，更安全 |

---

## 八、教学建议（课改思路）

1. **先 C 后 C++**：让学生先体会"手动管理内存的痛"，再用 C++ 的 RAII 化解
2. **画图教学**：链表插入/删除必须画图理解
3. **对比教学**：
   - 自实现 vs STL 容器
   - 普通指针 vs 智能指针
4. **可视化工具**：
   - [Visualgo](https://visualgo.net/) 演示算法过程
   - [C++ Insights](https://cppinsights.io/) 看模板展开
5. **项目式学习**：用模板线性表实现"通讯录管理系统"，支持任意数据类型
6. **思政融入**：
   - **RAII** 思想 ↔ "善始善终"
   - **封装** ↔ "合理分工"
   - **多态** ↔ "因材施教"

---

## 九、课后作业设计

| 题型 | 示例 |
|------|------|
| **概念题** | 解释 C++ 中"三法则"和"五法则"，并说明在线性表中如何体现 |
| **改错题** | 给出含浅拷贝 bug 的链表代码，让学生改正为深拷贝 |
| **算法题** | 用模板实现一个去重函数，能处理 `SqList<int>` 和 `SqList<string>` |
| **综合题** | 用链表 + 模板实现一元多项式 `P(x)` 的加减乘运算 |
| **拓展题** | 把 `LinkList` 改用 `std::shared_ptr` 重写，对比内存安全性 |
| **STL 题** | 用 `std::vector` 和 `std::list` 解决相同问题，对比性能 |

---

## 十、配套 C++ 工程结构

```
LinearList/
├── include/
│   ├── List.hpp            // 抽象基类（模板）
│   ├── SqList.hpp          // 顺序表（模板）
│   └── LinkList.hpp        // 链表（模板）
├── src/
│   └── main.cpp            // 测试主程序
├── tests/
│   └── test_list.cpp       // 单元测试
├── CMakeLists.txt          // CMake 构建脚本
└── README.md
```

### CMakeLists.txt 示例
```cmake
cmake_minimum_required(VERSION 3.10)
project(LinearList CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
add_executable(main src/main.cpp)
```

### 编译运行
```bash
mkdir build && cd build
cmake ..
make
./main
```

---

## 十一、与 C 版本的主要区别对比

| 特性 | C 语言 | C++ |
|------|--------|-----|
| **数据封装** | 结构体 + 散函数 | 类（class）|
| **泛型** | 不支持，需写多份代码 | 模板（template）|
| **内存管理** | `malloc / free` | `new / delete` 或智能指针 |
| **传参** | 指针 `SqList *L` | 引用 `SqList& L` 更优雅 |
| **错误处理** | 返回 0/-1 标志 | 异常 `throw / catch` |
| **代码复用** | 拷贝粘贴 | 继承 + 多态 |
| **资源管理** | 手动 free | RAII 自动管理 |
| **I/O** | `printf / scanf` | `cout / cin` 或 `fmt` |
| **NULL 值** | `NULL` | `nullptr`（类型安全）|
| **bool 类型** | `int 0/1` 或 `<stdbool.h>` | 内建 `bool true/false` |

---

## 十二、完整可运行示例（一个文件版）

```cpp
// linear_list.cpp
// 编译：g++ -std=c++17 linear_list.cpp -o linear_list
#include <iostream>
#include <stdexcept>

template <typename T>
class SqList {
private:
    T* data;
    int length, capacity;
public:
    explicit SqList(int cap = 100) : length(0), capacity(cap) {
        data = new T[capacity];
    }
    ~SqList() { delete[] data; }
    
    void insert(int i, const T& e) {
        if (i < 1 || i > length + 1) throw std::out_of_range("位置非法");
        if (length >= capacity) throw std::overflow_error("表已满");
        for (int j = length; j >= i; j--) data[j] = data[j - 1];
        data[i - 1] = e;
        length++;
    }
    
    void remove(int i) {
        if (i < 1 || i > length) throw std::out_of_range("位置非法");
        for (int j = i; j < length; j++) data[j - 1] = data[j];
        length--;
    }
    
    void traverse() const {
        for (int i = 0; i < length; i++) 
            std::cout << data[i] << " ";
        std::cout << std::endl;
    }
    
    int size() const { return length; }
    bool empty() const { return length == 0; }
};

int main() {
    try {
        // 测试 int 类型
        SqList<int> L1(10);
        for (int i = 1; i <= 5; i++) L1.insert(i, i * 10);
        std::cout << "整数顺序表：";
        L1.traverse();              // 10 20 30 40 50
        
        L1.remove(3);
        std::cout << "删除第 3 个元素后：";
        L1.traverse();              // 10 20 40 50
        
        // 模板的威力：同一份代码处理 string 类型
        SqList<std::string> L2(10);
        L2.insert(1, "Hello");
        L2.insert(2, "World");
        L2.insert(3, "C++");
        std::cout << "字符串顺序表：";
        L2.traverse();              // Hello World C++
        
    } catch (const std::exception& e) {
        std::cerr << "错误：" << e.what() << std::endl;
    }
    return 0;
}
```

### 编译运行
```bash
g++ -std=c++17 linear_list.cpp -o linear_list
./linear_list
```

### 预期输出
```
整数顺序表：10 20 30 40 50 
删除第 3 个元素后：10 20 40 50 
字符串顺序表：Hello World C++ 
```

---

## 十三、进阶：使用智能指针的现代 C++ 链表

为了让学生体会**现代 C++ 的资源管理优势**，下面给出一个**完全无需手动 `delete`** 的链表实现：

### 1. 节点定义（智能指针版）
```cpp
#include <memory>

template <typename T>
struct SmartNode {
    T data;
    std::unique_ptr<SmartNode<T>> next;     // 独占所有权
    
    explicit SmartNode(const T& e) : data(e), next(nullptr) {}
};
```

### 2. 链表类

```cpp
template <typename T>
class SmartLinkList {
private:
    std::unique_ptr<SmartNode<T>> head;
    int length;
    
public:
    SmartLinkList() : head(std::make_unique<SmartNode<T>>(T())), length(0) {}
    
    // 不需要写析构函数！智能指针自动释放整条链表
    
    void insert(int i, const T& e) {
        if (i < 1 || i > length + 1)
            throw std::out_of_range("位置非法");
        
        SmartNode<T>* p = head.get();
        for (int j = 1; j < i; j++) p = p->next.get();
        
        auto newNode = std::make_unique<SmartNode<T>>(e);
        newNode->next = std::move(p->next);
        p->next = std::move(newNode);
        length++;
    }
    
    void remove(int i) {
        if (i < 1 || i > length)
            throw std::out_of_range("位置非法");
        
        SmartNode<T>* p = head.get();
        for (int j = 1; j < i; j++) p = p->next.get();
        
        p->next = std::move(p->next->next);     // 旧节点自动释放
        length--;
    }
    
    void traverse() const {
        SmartNode<T>* p = head->next.get();
        while (p != nullptr) {
            std::cout << p->data << " ";
            p = p->next.get();
        }
        std::cout << std::endl;
    }
    
    int size() const { return length; }
};

```

### 3. 与传统指针版本对比

| 对比项 | 传统指针版 | 智能指针版 |
|--------|-----------|-----------|
| **代码量** | 多（要写析构、拷贝控制）| 少（编译器自动生成）|
| **内存安全** | 易泄漏、易野指针 | 自动释放，无泄漏 |
| **教学顺序** | 先讲，理解原理 | 后讲，体会进步 |
| **STL 风格** | 偏底层 | 接近现代 C++ 实践 |

> 💡 **教学建议**：第 1-2 节课先讲传统指针版，让学生体会内存管理的"痛"；第 3-4 节课引入智能指针版，让学生感受"工具进化"的力量。

---

## 十四、完整教学进度安排（8 课时）

| 课时 | 主题 | 内容要点 | 实验/作业 |
|------|------|---------|----------|
| **第 1 课时** | 线性表概念 | 定义、ADT、C++ 类设计 | 写出 ADT 类声明 |
| **第 2 课时** | 顺序表（一）| 类定义、构造析构、插入删除 | 实现 `SqList` 基本操作 |
| **第 3 课时** | 顺序表（二）| 拷贝控制、模板化、运算符重载 | 模板化改造 |
| **第 4 课时** | 链表（一）| 节点定义、单链表类、头/尾插法 | 实现 `LinkList` 基本操作 |
| **第 5 课时** | 链表（二）| 插入删除、链表逆置、双向链表 | 链表逆置 + 合并 |
| **第 6 课时** | 进阶专题 | 智能指针、移动语义、STL 对比 | 用 `vector`/`list` 重写 |
| **第 7 课时** | 综合应用 | 一元多项式、约瑟夫环、LRU | 综合项目 |
| **第 8 课时** | 实验 + 测验 | 项目演示、单元测试、答疑 | 提交完整工程 |

---

## 十五、推荐配套资源

### 📚 教材
- 《数据结构（C++ 语言版）》—— 邓俊辉（清华大学）
- 《C++ Primer (第 5 版)》—— Stanley B. Lippman
- 《Effective Modern C++》—— Scott Meyers

### 🌐 在线工具
- [Compiler Explorer](https://godbolt.org/) —— 在线对比不同编译器输出
- [Visualgo](https://visualgo.net/) —— 数据结构可视化
- [cppreference.com](https://cppreference.com/) —— C++ 标准查询

### 🎬 推荐视频
- B 站【清华大学 邓俊辉】数据结构课
- B 站【浙江大学 翁恺】C++ 程序设计

---

## 📌 总结：C 版 vs C++ 版核心差异

| 维度 | C 版 | C++ 版 |
|------|------|--------|
| **编程范式** | 面向过程 | 面向对象 + 泛型 + 范型 |
| **代码组织** | 函数 + 结构体 | 类 + 模板 |
| **内存管理** | `malloc/free` 手动 | `new/delete` 或智能指针 |
| **错误处理** | 返回值标志 | 异常机制 |
| **复用性** | 复制粘贴改类型 | 模板一份通用 |
| **教学重点** | 指针、内存模型 | 类设计、RAII、模板 |
| **难度** | 中（指针易错）| 较高（概念多）|
| **工业实用性** | 嵌入式、底层 | 应用开发、游戏、AI 框架 |

> ✨ **建议教学路径**：  
> **C 语言版** → 让学生体会底层原理与指针 → **C++ 版** → 引入面向对象与现代特性 → **STL** → 体会工业级实践

---

## 🎯 结语

C++ 版的"线性表"完整教学内容涵盖：

✅ 教学目标 & 课时安排  
✅ ADT 抽象基类设计  
✅ 顺序表完整模板类实现  
✅ 单链表 / 双向链表实现  
✅ 现代 C++ 智能指针进阶版  
✅ STL 容器对比  
✅ 典型例题与练习  
✅ 工程结构与 CMake 构建  
✅ 完整可运行示例代码

---

