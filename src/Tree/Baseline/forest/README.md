# 二叉树和森林的转换

> 森林是若干棵树的集合。通过左孩子右兄弟表示法，任意森林都可以唯一地映射为一棵二叉树。
>
> 森林的前序遍历等于对应二叉树的前序遍历，森林的后序遍历等于对应二叉树的中序遍历。

所有依赖代码位于 `Tree\Baseline\forest` 目录下。数据结构定义在 `forest.h`。

## 森林的定义

### 树和森林

树（Tree）是一种非线性数据结构，由节点和边组成，每个节点可以有零个或多个子节点。一棵树有且仅有一个根节点。

森林（Forest）则是若干棵互不相交的树的集合。森林中的每棵树都是一个独立的树结构，树根之间没有边相连。为便于处理，通常将森林定义为一个以各树根为元素的线性表。

### 二叉树

二叉树（Binary Tree）是每个节点最多有两个子节点（左孩子和右孩子）的树结构。由于二叉树的节点度数固定，它的存储和操作比一般树更为简单，因此在计算机科学中有着广泛的应用。

### 左孩子右兄弟表示法

左孩子右兄弟（Left-Child Right-Sibling, LCRS）表示法是一种将任意树（或森林）转换为二叉树的标准方法。其核心思想是：

- 对于树中的每个节点，将其**第一个孩子**作为它在二叉树中的**左孩子**。
- 将其**其余孩子**依次作为前一个兄弟节点的**右孩子**（即右兄弟链）。

对于森林，先转换第一棵树，其根作为二叉树的根；随后将森林中其余的树依次作为前一个树根的**右孩子**连接。

这种转换是**一一对应**的，也就是说，给定一棵二叉树，我们也可以唯一地还原出原始的森林。

## Baseline 任务: 二叉树与森林的转换

### 数据结构定义

本项目中使用以下数据结构来表示森林和二叉树（定义在 `forest.h`）：

#### 一般树节点（ForestNode）

```cpp
template <typename T>
struct ForestNode {
    T data;
    std::vector<std::unique_ptr<ForestNode>> children;

    ForestNode(const T &data);
    ForestNode(T &&data);
    static std::unique_ptr<ForestNode> get_new_node(const T &data);
    static std::unique_ptr<ForestNode> get_new_node(T &&data);
};
```

`ForestNode` 使用 `std::vector` 存储子节点，支持任意数量的孩子（可变度数）。所有权通过 `std::unique_ptr` 管理。

#### 森林（Forest）

```cpp
template <typename T>
using Forest = std::vector<std::unique_ptr<ForestNode<T>>>;
```

森林就是一棵棵树的根节点组成的向量。

#### 二叉树节点（BinaryTreeNode）

```cpp
template <typename T>
struct BinaryTreeNode {
    T data;
    std::unique_ptr<BinaryTreeNode> left;
    std::unique_ptr<BinaryTreeNode> right;

    BinaryTreeNode(const T &data);
    BinaryTreeNode(T &&data);
    static std::unique_ptr<BinaryTreeNode> get_new_node(const T &data);
    static std::unique_ptr<BinaryTreeNode> get_new_node(T &&data);
};
```

`BinaryTreeNode` 固定有 `left` 和 `right` 两个子节点指针，与一般二叉树节点定义一致。

### 森林转二叉树

函数签名：

```cpp
template <typename T>
std::unique_ptr<BinaryTreeNode<T>>
forest_to_binary_tree(const Forest<T> &forest);
```

算法步骤：

1. 若森林为空，返回空指针。
2. 对森林中的每棵树，递归转换每个子树：
    - 创建一个新的 `BinaryTreeNode`，数据与原节点相同。
    - 将原节点的**第一个孩子**递归转换为新节点的**左孩子**。
    - 将原节点的**后续孩子**依次链接为左孩子的**右兄弟链**（即每个兄弟作为前一个的 `right`）。
3. 森林中第一棵树的根成为结果二叉树的根。
4. 后续树的根依次作为前一树根的**右孩子**。

### 二叉树转森林

函数签名：

```cpp
template <typename T>
Forest<T>
binary_tree_to_forest(const std::unique_ptr<BinaryTreeNode<T>> &root);
```

算法步骤（`forest_to_binary_tree` 的逆过程）：

1. 若二叉树根为空，返回空森林。
2. 沿二叉树的**右链**向下遍历——右链上的每个节点都是森林中一棵树的根。
3. 对每个根节点，递归还原一般树：
    - 创建一个新的 `ForestNode`，数据与原节点相同。
    - 沿该节点的 `left`->`right` 链向下遍历——链上的每个节点都是该 `ForestNode` 的一个孩子。

### 森林的遍历

森林的遍历按顺序逐棵进行：

- **前序遍历**：对于森林中的每一棵树，先访问根节点，再依次递归访问每个子树。
- **后序遍历**：对于森林中的每一棵树，先依次递归访问每个子树，最后访问根节点。

关键不变式：**森林的前序遍历 = 二叉树的前序遍历**，**森林的后序遍历 = 二叉树的中序遍历**。

### 任务内容

学生需要在 `forest-impl.hpp` 中实现以下四个函数：

| 函数 | 说明 |
| --- | --- |
| `forest_to_binary_tree()` | 将森林转换为二叉树（左孩子右兄弟表示法） |
| `binary_tree_to_forest()` | 将二叉树还原为森林 |
| `forest_preorder()` | 森林的前序遍历 |
| `forest_postorder()` | 森林的后序遍历 |

你提交的材料中这部分内容需要包含：

- 四个函数的完整实现代码。
- 对左孩子右兄弟表示法原理的解释。
- 对森林与二叉树转换正确性的分析，特别是为什么森林的前序遍历等于二叉树的前序遍历，森林的后序遍历等于二叉树的中序遍历。
- 自行设计测试用例，验证转换的正确性和往返转换（round-trip）的一致性。

<details>
<summary>点击展开：Baseline 测试样例与预期输出</summary>

**注：**以下测试样例仅用于说明格式，不代表完整测试覆盖范围。学生需要根据该格式自行设计并生成更多测试样例，覆盖边界情况。

以 `forest-demo.cpp` 中的示例森林为例（两棵树：A 有孩子 B、C、D；E 有孩子 F、G）：

```text
Tree 1:     Tree 2:
    A            E
   /|\          / \
  B C D        F   G
```

#### 预期输出

```text
=== Original Forest ===
Forest preorder:  A B C D E F G
Forest postorder: B C D A F G E

=== Binary Tree (from forest) ===
Binary preorder:  A B C D E F G
Binary inorder:   B C D A F G E

=== Recovered Forest ===
Forest preorder:  A B C D E F G
Forest postorder: B C D A F G E
```

可以看到，**森林的前序遍历 = 二叉树的前序遍历**，**森林的后序遍历 = 二叉树的中序遍历**。往返转换后森林遍历结果与原始一致，验证了转换的正确性。

</details>
