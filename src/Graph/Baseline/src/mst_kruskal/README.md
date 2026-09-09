# 作业：Kruskal 最小生成树

本模块要求你实现 Kruskal 最小生成树算法。Kruskal 先按边权从小到大处理边，再依次加入不会形成环的边。

## 需要完成

请阅读 `mst_kruskal.hpp`，然后在 `mst_kruskal.cpp` 中补全：

- `DisjointSet::Find`
  - 查找顶点所在集合的代表元素。
  - 建议使用路径压缩。

- `DisjointSet::Union`
  - 判断两个顶点是否已经连通。
  - 如果不连通，则合并集合并返回 `true`。
  - 如果已经连通，说明加入该边会成环，返回 `false`。
  - 建议使用按秩合并。

- `Kruskal(edges, edge_count, vertex_count, mst_edges, total_cost)`
  - 按边权从小到大处理边。
  - 选择不会成环的边写入 `mst_edges[]`。
  - 累加总权重。
  - 连通图返回 `1`。
  - 不连通图或非法顶点编号返回 `0`。
  - 单顶点图返回 `1`，并令 `total_cost = 0`。

## 输入格式

`test.in` 第一行为测试用例数量。每个用例格式如下：

```text
vertex_count edge_count
from to weight
...
```

Kruskal 模块中的输入图视为无向带权图。

## 输出格式

测试程序会输出：

```text
CASE 1
OK 1
TOTAL 19
EDGE_COUNT 3
from to weight
...
```

`OK` 表示是否成功得到最小生成树。成功时，后续边应构成一棵合法 MST；失败时 `EDGE_COUNT` 为 `0`。

## 公开测试覆盖

- 连通图的 MST。
- 等权边场景。
- 不连通边集。
- 非法顶点编号。
- 单顶点图。
- 总权重、边数、连通性和成环检查。

## 编译运行

推荐在 `src/Graph/Baseline/` 目录运行：

```sh
make test-kruskal
```

也可以在本目录单独编译：

```sh
g++ -std=c++17 mst_kruskal.cpp test.cpp -o test_kruskal
./test_kruskal test.in
```

## 提交前自查

- 边是否按权重从小到大处理。
- `Find` 是否能找到集合代表。
- `Union` 是否能识别成环边。
- 是否只选入 `vertex_count - 1` 条边。
- 不连通图是否能返回 `0`。
