#ifndef DSAAC_SHORTEST_PATH_HPP
#define DSAAC_SHORTEST_PATH_HPP

/*
 * 使用 BFS 计算无权图上从 src 出发到各顶点的最短路径。
 *
 * dist[v]：src 到 v 的最短距离，不可达时为 -1。
 * prev[v]：最短路径上 v 的前驱顶点，无前驱时为 -1。
 */

struct GraphRecord;
using Graph = GraphRecord *;

Graph CreateShortestPathGraph(int vertex_count, int directed);
void DisposeShortestPathGraph(Graph graph);
void AddShortestPathEdge(Graph graph, int from, int to);
void ShortestPath(Graph graph, int src, int *dist, int *prev);

#endif
