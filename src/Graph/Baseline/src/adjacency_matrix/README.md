# 作业：邻接矩阵图

本模块要求你用邻接矩阵实现图的基础遍历。邻接矩阵适合帮助你理解 `matrix[i][j]` 如何表示边，并和邻接表进行对比。

## 需要完成

请阅读 `graph.hpp`，然后在 `graph.cpp` 中补全：

- `AdjMatrixBFS`
  - 从 `start` 出发做广度优先遍历。
  - 按顶点编号从小到大扫描邻接点。
  - 将访问顺序写入 `order[]`。

- `DFSRecursive`
  - 使用递归完成深度优先遍历。
  - 每个顶点只访问一次。
  - 按顶点编号从小到大扫描邻接点。

- `AdjMatrixTopologicalSort`
  - 根据矩阵统计入度。
  - 使用队列维护入度为 0 的顶点。
  - 有向无环图返回 `1`，有环图返回 `0`。
  - 对无向图调用时返回 `0`，并将 `count` 置为 `0`。

## 输入格式

`test.in` 第一行为测试用例数量。每个用例格式如下：

```text
vertex_count edge_count directed start
from to
...
```

其中 `directed = 1` 表示有向图，`directed = 0` 表示无向图。邻接矩阵遍历时按顶点编号从小到大扫描。

## 输出格式

测试程序会输出：

```text
CASE 1
BFS ...
DFS ...
TOPO_OK 1
TOPO ...
```

`TOPO_OK` 为 `1` 时，`TOPO` 后面是一个合法拓扑序；`TOPO_OK` 为 `0` 时，`TOPO` 为空。

## 公开测试覆盖

- 有向无环图上的 BFS、DFS、拓扑排序。
- 有向环图的拓扑排序失败。
- 无向图调用拓扑排序失败。
- 不连通图中只访问起点所在连通分量。
- 单顶点图。

## 编译运行

推荐在 `src/Graph/Baseline/` 目录运行：

```sh
make test-adj-matrix
```

也可以在本目录单独编译：

```sh
g++ -std=c++17 graph.cpp testgraph.cpp -o test_adj_matrix
./test_adj_matrix test.in
```

## 提交前自查

- 扫描邻接点时是否按编号从小到大处理。
- BFS 是否正确维护队列。
- DFS 是否正确避免重复访问。
- 拓扑排序是否正确统计和更新入度。
- 能否说明邻接矩阵和邻接表在时间复杂度上的区别。
