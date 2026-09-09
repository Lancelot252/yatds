# 作业：邻接表图

本模块要求你用邻接表实现图的基础遍历。邻接表适合表示稀疏图，也是后续最短路径和生成树算法常用的图存储方式。

## 需要完成

请阅读 `graph.hpp`，然后在 `graph.cpp` 中补全：

- `AdjListBFS`
  - 从 `start` 出发做广度优先遍历。
  - 将访问顺序写入 `order[]`。
  - 将访问到的顶点数量写入 `count`。

- `DFSRecursive`
  - 使用递归完成深度优先遍历。
  - 每个顶点只访问一次。
  - 将访问顺序写入 `order[]`。

- `AdjListTopologicalSort`
  - 使用入度数组和队列完成 Kahn 拓扑排序。
  - 有向无环图返回 `1`。
  - 有环图返回 `0`。
  - 对无向图调用时返回 `0`，并将 `count` 置为 `0`。

## 输入格式

`test.in` 第一行为测试用例数量。每个用例格式如下：

```text
vertex_count edge_count directed start
from to
...
```

其中 `directed = 1` 表示有向图，`directed = 0` 表示无向图。邻接表按加边顺序枚举邻接点。

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
make test-adj-list
```

也可以在本目录单独编译：

```sh
g++ -std=c++17 graph.cpp testgraph.cpp -o test_adj_list
./test_adj_list test.in
```

## 提交前自查

- BFS 是否正确使用队列。
- DFS 是否正确标记已访问顶点。
- 拓扑排序是否正确维护入度。
- 有环图是否不会输出完整拓扑序。
- 不连通图是否不会误访问其他连通分量。
