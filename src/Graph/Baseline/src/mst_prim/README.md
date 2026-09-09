# 作业：Prim 最小生成树

本模块要求你实现 Prim 最小生成树算法。Prim 从一个顶点开始，逐步把新的顶点接入当前生成树，每次选择连接代价最小的一条边。

## 需要完成

请阅读 `mst_prim.hpp`，然后在 `mst_prim.cpp` 中补全：

- `Prim(graph, mst_edges, total_cost)`
  - 从 0 号顶点开始。
  - 每轮选择一个未加入生成树、连接代价最小的顶点。
  - 将选中的边写入 `mst_edges[]`。
  - 累加生成树总权重到 `total_cost`。
  - 连通图返回 `1`。
  - 不连通图返回 `0`。
  - 单顶点图返回 `1`，并令 `total_cost = 0`。

建图接口已经给出：

- `CreatePrimGraph`
- `DisposePrimGraph`
- `AddPrimEdge`

## 输入格式

`test.in` 第一行为测试用例数量。每个用例格式如下：

```text
vertex_count edge_count
from to weight
...
```

Prim 模块中的输入图视为无向带权图。

## 输出格式

测试程序会输出：

```text
CASE 1
OK 1
TOTAL 16
EDGE_COUNT 4
from to weight
...
```

`OK` 表示是否成功得到最小生成树。成功时，后续边应构成一棵合法 MST；失败时 `EDGE_COUNT` 为 `0`。

## 公开测试覆盖

- 连通图的 MST。
- 等权边场景。
- 不连通图。
- 单顶点图。
- 总权重、边数、连通性和成环检查。

## 编译运行

推荐在 `src/Graph/Baseline/` 目录运行：

```sh
make test-prim
```

也可以在本目录单独编译：

```sh
g++ -std=c++17 mst_prim.cpp test.cpp -o test_prim
./test_prim test.in
```

## 提交前自查

- 是否从 0 号顶点开始扩展。
- 每轮是否选择了当前可连接的最小权重边。
- 是否正确记录父节点或连接边。
- 不连通图是否能返回 `0`。
- 最终边数是否为 `vertex_count - 1`。
