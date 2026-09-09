#ifndef DSAAC_DIJKSTRA_HPP
#define DSAAC_DIJKSTRA_HPP

/*
 * 带权邻接表图，用于 Dijkstra 和后续需要权重的算法。
 * 与 adjacency_list/graph.hpp 的无权版本是独立的两套接口。
 */

struct WGraphRecord;
using WGraph = WGraphRecord *;

WGraph CreateDijkstraGraph(int vertex_count, int directed);
void DisposeDijkstraGraph(WGraph graph);
void AddDijkstraEdge(WGraph graph, int from, int to, int weight);

/*
 * Dijkstra 单源最短路径（非负权图）。
 *
 * dist[v]  ：src 到 v 的最短距离，不可达时为 -1。
 * prev[v]  ：最短路径上 v 的前驱顶点，无前驱时为 -1。
 * 调用方负责分配 dist 和 prev 数组（长度 = vertex_count）。
 */
void Dijkstra(WGraph graph, int src, int *dist, int *prev);

#endif
