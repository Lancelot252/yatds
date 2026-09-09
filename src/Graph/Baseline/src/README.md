# 图 Baseline 实验

## 图的表示、遍历与拓扑排序

> 同一张图可以用不同的存储方式表达。选择表示法，会直接影响遍历边、判断相邻关系和空间占用的成本。

### 邻接表与邻接矩阵

| 表示 | 核心结构 | 空间 | 适合场景 |
| --- | --- | --- | --- |
| 邻接表 | 每个顶点保存一组相邻顶点 | O(V + E) | 稀疏图、需要遍历出边 |
| 邻接矩阵 | `matrix[u][v]` 表示边 | O(V²) | 稠密图、频繁判断两点是否相邻 |

```text
0 ── 1        邻接表              邻接矩阵
│  ╱ │        0: 1, 2            0 1 1 0
│ ╱  │        1: 0, 2, 3         1 0 1 1
2 ── 3        2: 0, 1, 3         1 1 0 1
              3: 1, 2            0 1 1 0
```

### BFS 与 DFS

| 算法 | 核心结构 | 访问方式 | 典型用途 |
| --- | --- | --- | --- |
| BFS | 队列 | 按层向外扩展 | 无权最短路、层级关系 |
| DFS | 递归或栈 | 沿一条路径深入后回溯 | 连通性、路径枚举、拓扑相关问题 |

无论使用哪种表示，都必须维护 `visited[]`，确保每个顶点最多访问一次。邻接表按加边顺序枚举邻接点；本作业的邻接矩阵按顶点编号从小到大扫描。

### Kahn 拓扑排序

1. 统计有向图中每个顶点的入度。
2. 把全部入度为 0 的顶点加入队列。
3. 依次取出顶点并删除其出边；新的入度 0 顶点继续入队。
4. 若最终输出顶点数小于 V，说明图中存在环。

> **边界约定：**无向图调用拓扑排序时返回失败；有环图不应输出一个“看似完整”的拓扑序。

### Baseline 任务

分别在 `adjacency_list/` 与 `adjacency_matrix/` 中补全 BFS、DFS 和拓扑排序，保持头文件中的接口签名不变。

#### 统一输入

```text
case_count
vertex_count edge_count directed start
from to
...
```

#### 统一输出

```text
CASE 1
BFS ...
DFS ...
TOPO_OK 1
TOPO ...
```

| 检查项 | 要求 |
| --- | --- |
| 顶点编号 | `0..vertex_count-1` |
| 方向 | `directed = 1` 为有向图，`0` 为无向图 |
| 遍历范围 | 不连通图只访问起点所在连通分量 |
| 测试 | `make test-adj-list` 与 `make test-adj-matrix` |

## 图的最短路径

> “最短”必须先定义：是经过的边最少，还是边权之和最小？目标不同，建模和算法也不同。

### 无权图：BFS 单源最短路

当每条边代价相同时，BFS 按距离一层层扩展。顶点第一次被访问时，就已经得到从源点出发的最少边数。

```text
dist[src] = 0
queue.push(src)
while queue 非空:
    u = queue.pop()
    for v in u 的邻接点:
        if dist[v] == -1:
            dist[v] = dist[u] + 1
            prev[v] = u
            queue.push(v)
```

`dist[v]` 保存最少边数；`prev[v]` 保存最短路径上的前驱。起点及不可达顶点的前驱均为 `-1`。

### 非负权图：Dijkstra

Dijkstra 每轮确定一个当前距离最小的未确定顶点，再使用它的出边进行松弛：

```text
if dist[u] + weight(u, v) < dist[v]:
    dist[v] = dist[u] + weight(u, v)
    prev[v] = u
```

算法成立的关键是所有边权非负。若允许负权边，一个已经“确定”的最短距离仍可能被后续路径改小。

### 算法选择

| 问题 | 推荐算法 | 判断依据 |
| --- | --- | --- |
| 最少经过几条边 | BFS 最短路 | 所有边代价相同 |
| 非负边权总代价最低 | Dijkstra | 路径代价由边权累加 |
| 还原具体路线 | 回溯 `prev[]` | 从终点反向追到源点后翻转 |
| 不可达顶点 | 保留 `-1` | 不能伪造距离或前驱 |

### Baseline 任务

#### 任务 1：`ShortestPath`

在 `unweighted_shortest_path/` 中基于 BFS 补全 `dist[]` 和 `prev[]`。

```text
vertex_count edge_count directed source
from to
...
```

#### 任务 2：`Dijkstra`

在 `dijkstra/` 中完成最小顶点选择和边松弛，同步更新前驱。

```text
vertex_count edge_count directed source
from to weight
...
```

#### 输出与测试

```text
CASE 1
DIST ...
PREV ...
```

运行 `make test-shortest-path` 与 `make test-dijkstra`。公开测试覆盖有向/无向图、不可达点、单顶点图以及 `dist[]` 与 `prev[]` 的一致性。

## 最小生成树：Prim 与 Kruskal

> 在连通无向带权图中，用 V−1 条边连接全部顶点且总权重最小，这棵树就是最小生成树（MST）。

### 问题定义与边界

- MST 不允许成环，最终必须连接所有顶点。
- 成功结果恰好包含 `vertex_count - 1` 条边。
- 不连通图不存在生成树，应返回失败。
- 单顶点图返回成功，总权重为 0，边数为 0。

### Prim：扩展一棵树

Prim 从 0 号顶点开始，维护“未加入顶点连接到当前生成树的最小代价”，每轮选取代价最小的顶点并记录对应连接边。

| 状态 | 含义 |
| --- | --- |
| `in_mst[v]` | 顶点是否已进入生成树 |
| `key[v]` | 从当前生成树连接到 v 的最小代价 |
| `parent[v]` | 提供该最小代价的树内顶点 |

### Kruskal：筛选一组边

1. 把所有边按权重从小到大排序。
2. 依次考虑每条边；若两端属于不同连通分量，则选入。
3. 通过并查集的 `Find` 与 `Union` 判断是否会成环。
4. 选满 V−1 条边时完成。

建议同时实现路径压缩与按秩合并，使并查集操作接近常数时间。

| 对比 | Prim | Kruskal |
| --- | --- | --- |
| 观察对象 | 顶点与当前树的连接代价 | 全局排序后的边 |
| 核心结构 | 最小代价数组/优先队列 | 排序 + 并查集 |
| 直观优势 | 适合从某点逐步扩展 | 适合边集清晰的稀疏图 |

### Baseline 任务

在 `mst_prim/` 中补全 `Prim`；在 `mst_kruskal/` 中补全并查集和 `Kruskal`。

#### 输入

```text
case_count
vertex_count edge_count
from to weight
...
```

#### 输出

```text
CASE 1
OK 1
TOTAL 16
EDGE_COUNT 4
from to weight
...
```

运行 `make test-prim` 与 `make test-kruskal`。测试不仅比较总权重，也会检查边数、连通性和是否成环；同权图可能存在多棵合法 MST。
