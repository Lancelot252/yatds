# 作业：无权图最短路径

本模块要求你在无权图上实现单源最短路径。这里的最短指经过的边数最少，因此可以在 BFS 的基础上完成。

## 需要完成

请阅读 `shortest_path.hpp`，然后在 `shortest_path.cpp` 中补全：

- `ShortestPath(graph, src, dist, prev)`
  - 从 `src` 出发进行 BFS。
  - 第一次访问到顶点 `v` 时，设置 `dist[v]`。
  - 同时记录 `prev[v]`，表示最短路径上 `v` 的前驱顶点。
  - 不可达顶点保持 `dist[v] = -1`、`prev[v] = -1`。

建图接口已经给出：

- `CreateShortestPathGraph`
- `DisposeShortestPathGraph`
- `AddShortestPathEdge`

## 输入格式

`test.in` 第一行为测试用例数量。每个用例格式如下：

```text
vertex_count edge_count directed source
from to
...
```

其中 `source` 是单源最短路径的起点。

## 输出格式

测试程序会输出：

```text
CASE 1
DIST ...
PREV ...
```

`DIST` 是起点到每个顶点的最短距离；不可达为 `-1`。`PREV` 是最短路径前驱；起点和不可达点为 `-1`。

## 公开测试覆盖

- 有向图上的最短路径。
- 无向图上的最短路径。
- 不可达顶点。
- 单顶点图。
- `dist[]` 和 `prev[]` 的一致性。

## 编译运行

推荐在 `src/Graph/Baseline/` 目录运行：

```sh
make test-shortest-path
```

也可以在本目录单独编译：

```sh
g++ -std=c++17 shortest_path.cpp test.cpp -o test_sp
./test_sp test.in
```

## 提交前自查

- BFS 队列是否正确初始化。
- 是否只在第一次访问顶点时设置 `dist[]` 和 `prev[]`。
- 起点是否满足 `dist[source] = 0`、`prev[source] = -1`。
- 不可达顶点是否保持 `-1`。
- 能否根据 `prev[]` 还原一条最短路径。
