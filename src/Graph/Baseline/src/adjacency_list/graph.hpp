#ifndef DSAAC_ADJ_LIST_GRAPH_HPP
#define DSAAC_ADJ_LIST_GRAPH_HPP

struct AdjListGraphRecord;
using AdjListGraph = AdjListGraphRecord *;

AdjListGraph CreateAdjListGraph(int vertex_count, int directed);
void DisposeAdjListGraph(AdjListGraph graph);
void AddAdjListEdge(AdjListGraph graph, int from, int to);
void PrintAdjListGraph(AdjListGraph graph);

void AdjListBFS(AdjListGraph graph, int start, int *order, int *count);
void AdjListDFS(AdjListGraph graph, int start, int *order, int *count);

int AdjListTopologicalSort(AdjListGraph graph, int *order, int *count);

#endif
