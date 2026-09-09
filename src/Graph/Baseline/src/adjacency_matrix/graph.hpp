#ifndef DSAAC_ADJ_MATRIX_GRAPH_HPP
#define DSAAC_ADJ_MATRIX_GRAPH_HPP

struct AdjMatrixGraphRecord;
using AdjMatrixGraph = AdjMatrixGraphRecord *;

AdjMatrixGraph CreateAdjMatrixGraph(int vertex_count, int directed);
void DisposeAdjMatrixGraph(AdjMatrixGraph graph);
void AddAdjMatrixEdge(AdjMatrixGraph graph, int from, int to);
void PrintAdjMatrixGraph(AdjMatrixGraph graph);

void AdjMatrixBFS(AdjMatrixGraph graph, int start, int *order, int *count);
void AdjMatrixDFS(AdjMatrixGraph graph, int start, int *order, int *count);

int AdjMatrixTopologicalSort(AdjMatrixGraph graph, int *order, int *count);

#endif
