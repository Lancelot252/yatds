#ifndef DSAAC_MST_PRIM_HPP
#define DSAAC_MST_PRIM_HPP

#include "../edge.hpp"

struct PrimGraphRecord;
using PrimGraph = PrimGraphRecord *;

PrimGraph CreatePrimGraph(int vertex_count);
void DisposePrimGraph(PrimGraph graph);
void AddPrimEdge(PrimGraph graph, int from, int to, int weight);

/*
 * Prim 最小生成树。
 *
 * mst_edges：调用方分配，长度至少为 vertex_count - 1。
 * total_cost：返回 MST 总权重。
 * 返回值：成功返回 1；图不连通或输入不合法返回 0。
 */
int Prim(PrimGraph graph, Edge *mst_edges, int *total_cost);

#endif
