#ifndef DSAAC_MST_KRUSKAL_HPP
#define DSAAC_MST_KRUSKAL_HPP

#include "../edge.hpp"

/*
 * Kruskal 最小生成树。
 *
 * edges      ：所有边的数组（调用方传入）。
 * edge_count ：边的总数。
 * vertex_count：顶点总数。
 * mst_edges  ：输出数组，长度至少为 vertex_count - 1（调用方分配）。
 * total_cost ：返回 MST 总权重。
 * 返回值：成功返回 1；图不连通返回 0。
 */
int Kruskal(Edge *edges, int edge_count, int vertex_count, Edge *mst_edges, int *total_cost);

#endif
