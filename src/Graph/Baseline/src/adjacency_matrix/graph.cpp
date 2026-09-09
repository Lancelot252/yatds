#include "graph.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

struct AdjMatrixGraphRecord
{
    int VertexCount;
    bool Directed;
    std::vector<int> Matrix;
};

#define CELL(g, from, to) ((g)->Matrix[(from) * (g)->VertexCount + (to)])

static void Die(const char *message)
{
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
}

static void ValidateVertex(AdjMatrixGraph graph, int vertex)
{
    if (vertex < 0 || vertex >= graph->VertexCount) {
        Die("Vertex index out of range");
    }
}

AdjMatrixGraph CreateAdjMatrixGraph(int vertex_count, int directed)
{
    if (vertex_count <= 0) {
        Die("Vertex count must be positive");
    }

    return new AdjMatrixGraphRecord{
        vertex_count,
        directed != 0,
        std::vector<int>(vertex_count * vertex_count, 0),
    };
}

void DisposeAdjMatrixGraph(AdjMatrixGraph graph)
{
    delete graph;
}

void AddAdjMatrixEdge(AdjMatrixGraph graph, int from, int to)
{
    if (graph == nullptr) {
        Die("Graph must not be NULL");
    }
    ValidateVertex(graph, from);
    ValidateVertex(graph, to);

    CELL(graph, from, to) = 1;
    if (!graph->Directed) {
        CELL(graph, to, from) = 1;
    }
}

void PrintAdjMatrixGraph(AdjMatrixGraph graph)
{
    if (graph == nullptr) {
        Die("Graph must not be NULL");
    }

    for (int i = 0; i < graph->VertexCount; ++i) {
        std::cout << i << ':';
        for (int j = 0; j < graph->VertexCount; ++j) {
            if (CELL(graph, i, j)) {
                std::cout << ' ' << j;
            }
        }
        std::cout << '\n';
    }
}

void AdjMatrixBFS(AdjMatrixGraph graph, int start, int *order, int *count)
{
    if (graph == nullptr || order == nullptr || count == nullptr) {
        Die("AdjMatrixBFS received NULL argument");
    }
    ValidateVertex(graph, start);

    std::vector<int> visited(graph->VertexCount, 0);
    std::vector<int> queue(graph->VertexCount, 0);
    int front = 0;
    int back = 0;

    *count = 0;
    visited[start] = 1;
    queue[back++] = start;

    /*
     * TODO(student):
     * 1. 当 front < back 时，取出队首顶点 vertex。
     * 2. 将 vertex 写入 order，并更新 count。
     * 3. 扫描矩阵中 vertex 这一行，找出所有相邻顶点 j。
     * 4. 第一次遇到 j 时，标记 visited[j] 并入队。
     */
    (void)visited;
    (void)queue;
    (void)front;
    (void)back;
    (void)order;
}

static void DFSRecursive(AdjMatrixGraph graph, int vertex, std::vector<int> &visited, int *order, int *count)
{
    /*
     * TODO(student):
     * 1. 标记 vertex 已访问。
     * 2. 将 vertex 写入 order，并更新 count。
     * 3. 扫描矩阵中 vertex 这一行。
     * 4. 对尚未访问的相邻顶点递归调用 DFSRecursive。
     */
    (void)graph;
    (void)vertex;
    (void)visited;
    (void)order;
    (void)count;
}

void AdjMatrixDFS(AdjMatrixGraph graph, int start, int *order, int *count)
{
    if (graph == nullptr || order == nullptr || count == nullptr) {
        Die("AdjMatrixDFS received NULL argument");
    }
    ValidateVertex(graph, start);

    std::vector<int> visited(graph->VertexCount, 0);
    *count = 0;
    DFSRecursive(graph, start, visited, order, count);
}

int AdjMatrixTopologicalSort(AdjMatrixGraph graph, int *order, int *count)
{
    if (graph == nullptr || order == nullptr || count == nullptr) {
        Die("AdjMatrixTopologicalSort received NULL argument");
    }

    if (!graph->Directed) {
        *count = 0;
        return 0;
    }

    *count = 0;

    /*
     * TODO(student):
     * 1. 扫描矩阵，统计每个顶点的入度 indegree[]。
     * 2. 将所有入度为 0 的顶点加入队列。
     * 3. 反复取出队首顶点，写入 order。
     * 4. 扫描该顶点对应的矩阵行，更新相邻顶点入度。
     * 5. 若最终输出顶点数等于 graph->VertexCount，返回 1，否则返回 0。
     */
    (void)order;
    return 0;
}
