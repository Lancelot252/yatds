# 作业：Dijkstra 最短路径

本模块要求你在非负权图上实现 Dijkstra 单源最短路径算法。和无权最短路径不同，这里比较的是路径权重之和。

## 需要完成

请阅读 `dijkstra.hpp`，然后在 `dijkstra.cpp` 中补全：

- `Dijkstra(graph, src, dist, prev)`
  - 初始化距离和前驱数组。
  - 每轮选择一个尚未确定、当前距离最小的顶点。
  - 遍历该顶点的出边并尝试松弛。
  - 松弛成功时同步更新 `prev[]`。
  - 不可达顶点保持 `dist[v] = -1`、`prev[v] = -1`。

建图接口已经给出：

- `CreateDijkstraGraph`
- `DisposeDijkstraGraph`
- `AddDijkstraEdge`

## 输入格式

`test.in` 第一行为测试用例数量。每个用例格式如下：

```text
vertex_count edge_count directed source
from to weight
...
```

本作业只处理非负权边。

## 输出格式

测试程序会输出：

```text
CASE 1
DIST ...
PREV ...
```

`DIST` 是起点到每个顶点的最短距离；不可达为 `-1`。`PREV` 是最短路径前驱；起点和不可达点为 `-1`。

## 公开测试覆盖

- 有向非负权图。
- 无向非负权图。
- 松弛后选择更短路径。
- 不可达顶点。
- 单顶点图。
- `dist[]` 和 `prev[]` 的一致性。

## 编译运行

推荐在 `src/Graph/Baseline/` 目录运行：

```sh
make test-dijkstra
```

也可以在本目录单独编译：

```sh
g++ -std=c++17 dijkstra.cpp test.cpp -o test_dijkstra
./test_dijkstra test.in
```

## 提交前自查

- 是否每轮选择了当前未确定顶点中距离最小的一个。
- 是否正确跳过不可达顶点。
- 松弛公式是否正确。
- `prev[]` 是否能还原一条最短路径。
- 是否能说明为什么 Dijkstra 不适用于负权边。
