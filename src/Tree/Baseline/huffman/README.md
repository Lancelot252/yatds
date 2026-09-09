# 哈夫曼树

> 哈夫曼树是一种带权路径长度最短的二叉树，也称为最优二叉树。
>
> 核心思想：频率高的字符用短编码，频率低的字符用长编码，从而实现数据压缩。

依赖代码位于 `Tree\Baseline\huffman` 和 `Tree\Project\huffman` 目录下。

## Baseline 任务: 哈夫曼树的基本实现

### 什么是哈夫曼树

在数据通信中，我们希望用尽可能短的二进制编码来表示字符。如果使用固定长度编码（如 ASCII 每字符 8 位），则无法利用字符出现频率的差异。哈夫曼编码（Huffman Coding）是一种可变长度编码方法，根据字符出现的频率分配不同长度的编码，使得总体编码长度最短。

设一组字符的权值（频率）为 $w_1, w_2, \dots, w_n$，对应的哈夫曼树中每个叶子节点到根的路径长度为 $l_i$，则树的**带权路径长度**（WPL）为：

$$WPL = \sum_{i=1}^{n} w_i \cdot l_i$$

哈夫曼树就是使 WPL 最小的二叉树。

哈夫曼树具有以下性质：

- 每个叶子节点对应一个原始字符（带权值）。
- 每个内部节点不存储字符，其权值为两个子节点权值之和。
- 从根到叶子的路径定义了该字符的编码：向左走为 `0`，向右走为 `1`。
- 没有编码是另一个编码的前缀（前缀码），保证了解码的唯一性。

### 数据结构定义

本项目中使用以下数据结构（定义在 `huffman-tree.h`）：

#### 通用 N 叉树节点（Node）

```cpp
template <typename T, std::size_t N = 2>
struct Node {
    T data;
    std::array<std::unique_ptr<Node>, N> children;

    Node(const T &data);
    Node(T &&data);
    static std::unique_ptr<Node> get_new_node(const T &data);
    static std::unique_ptr<Node> get_new_node(T &&data);
};
```

`Node` 是一个模板化的通用节点，`N` 为子节点数量（默认为 2，即二叉树节点）。子节点存储在固定大小的 `std::array` 中，所有权通过 `std::unique_ptr` 管理。

#### 哈夫曼树节点数据（HuffmanTreeNodeData）

```cpp
template <typename data_t, typename weight_t>
struct HuffmanTreeNodeData {
    std::optional<data_t> data;   // 叶子节点存储字符，内部节点为 nullopt
    weight_t weight;              // 权值（频率）
};
```

`HuffmanTreeNodeData` 使用 `std::optional<data_t>` 来区分叶子节点（有字符数据）和内部节点（无字符数据）。

#### 哈夫曼树节点类型别名

```cpp
template <typename data_t, typename weight_t>
using HuffmanTreeNode = Node<HuffmanTreeNodeData<data_t, weight_t>, 2>;
```

`HuffmanTreeNode` 是一个携带 `HuffmanTreeNodeData` 作为数据载荷的二叉树节点。

### 构造算法

函数签名：

```cpp
template <typename data_t, typename weight_t>
std::unique_ptr<HuffmanTreeNode<data_t, weight_t>>
construct_huffman_tree(const std::vector<std::pair<data_t, weight_t>> &leaves);
```

算法步骤（贪心 + 最小堆）：

1. 若输入为空，返回空指针。
2. 为每个 `(data, weight)` 对创建一个叶子节点。
3. 将所有节点放入一个**最小堆**（按权值排序），使用 `std::make_heap` 和自定义比较器。
4. 当堆中节点数大于 1 时，重复以下步骤：
    - 从堆中弹出两个权值最小的节点（`node1` 和 `node2`）。
    - 创建一个新的内部节点，其权值为 `node1->data.weight + node2->data.weight`，`data` 字段为 `std::nullopt`。
    - 将 `node1` 设为左孩子（`children[0]`），`node2` 设为右孩子（`children[1]`）。
    - 将新节点压回堆中。
5. 堆中仅剩的节点即为哈夫曼树的根。

时间复杂度：$O(n \log n)$，其中 n 为叶子节点数量。每次堆操作（pop 和 push）为 $O(\log n)$，共执行 $n-1$ 次合并。

### 哈夫曼编码

构造好哈夫曼树后，可以通过 DFS 遍历树来生成每个字符的编码。从根节点出发，沿途记录路径：向左走记为 `"0"`，向右走记为 `"1"`。当到达叶子节点时，当前路径字符串即为该字符的哈夫曼编码。

评分依据为经编码后的整个字符串的二进制比特流长度，不要求编码与参考实现完全一致（只要 WPL 相同即可）。

### 任务内容

学生需要在 `huffman-tree-impl.hpp` 中实现 `construct_huffman_tree()` 函数，使用输入的频率数据构建哈夫曼树。

你提交的材料中这部分内容需要包含：

- `construct_huffman_tree()` 的完整实现代码。
- 对哈夫曼树构造算法（贪心策略 + 最小堆）的原理说明。
- 对算法时间复杂度的分析（$O(n \log n)$ 的推导过程）。
- 自行设计测试用例，包含不同字符频率分布（均匀分布、极端分布等），测试编码结果并计算 WPL。
- 分析为什么哈夫曼编码是前缀码，以及为什么它能保证最优压缩。

<details>
<summary>点击展开：Baseline 测试样例与预期输出</summary>

**注：**以下测试样例仅用于说明格式，不代表完整测试覆盖范围。学生需要根据该格式自行设计并生成更多测试样例，覆盖边界情况。

`huffman-coding.cpp` 是一个交互式测试程序，读取一行文本，统计字符频率，构建哈夫曼树，并输出每个字符的编码。

#### 输入样例

```
Enter one line of string: hello world
```

#### 输出样例

```text
char   cnt 1
char d cnt 1
char e cnt 1
char h cnt 1
char r cnt 1
char w cnt 1
char o cnt 2
char l cnt 3
char   code 000
char d code 001
char e code 010
char h code 011
char r code 100
char w code 101
char o code 11
char l code 10
```

**评分标准：**以编码后整个字符串的二进制比特流总长度作为评分依据。编码结果不要求与参考实现完全一致，只要 WPL 相同（即总比特流长度相同）即可。

</details>
