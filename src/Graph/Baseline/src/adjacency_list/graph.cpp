#include "graph.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

struct AdjNode
{
    int Vertex;
    AdjNode *Next;
};

struct AdjListGraphRecord
{
    int VertexCount;
    bool Directed;
    std::vector<AdjNode *> Heads;
    std::vector<AdjNode *> Tails;
};

static void Die(const char *message)
{
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
}

static void ValidateVertex(AdjListGraph graph, int vertex)
{
    if (vertex < 0 || vertex >= graph->VertexCount) {
        Die("Vertex index out of range");
    }
}

static void AddDirectedEdge(AdjListGraph graph, int from, int to)
{
    AdjNode *node = new AdjNode{to, nullptr};

    if (graph->Heads[from] == nullptr) {
        graph->Heads[from] = node;
    } else {
        graph->Tails[from]->Next = node;
    }
    graph->Tails[from] = node;
}

static void DFSRecursive(AdjListGraph graph, int vertex, std::vector<int> &visited, int *order, int *count)
{
    /*
     * TODO(student):
     * 1. 标记 vertex 已访问。
     * 2. 将 vertex 写入 order，并更新 count。
     * 3. 沿 graph->Heads[vertex] 遍历所有邻接点。
     * 4. 对尚未访问的邻接点递归调用 DFSRecursive。
     */
    (void)graph;
    (void)vertex;
    (void)visited;
    (void)order;
    (void)count;
}

AdjListGraph CreateAdjListGraph(int vertex_count, int directed)
{
    if (vertex_count <= 0) {
        Die("Vertex count must be positive");
    }

    return new AdjListGraphRecord{
        vertex_count,
        directed != 0,
        std::vector<AdjNode *>(vertex_count, nullptr),
        std::vector<AdjNode *>(vertex_count, nullptr),
    };
}

void DisposeAdjListGraph(AdjListGraph graph)
{
    if (graph == nullptr) {
        return;
    }

    for (int i = 0; i < graph->VertexCount; ++i) {
        AdjNode *current = graph->Heads[i];
        while (current != nullptr) {
            AdjNode *next = current->Next;
            delete current;
            current = next;
        }
    }

    delete graph;
}

void AddAdjListEdge(AdjListGraph graph, int from, int to)
{
    if (graph == nullptr) {
        Die("Graph must not be NULL");
    }

    ValidateVertex(graph, from);
    ValidateVertex(graph, to);

    AddDirectedEdge(graph, from, to);
    if (!graph->Directed) {
        AddDirectedEdge(graph, to, from);
    }
}

void PrintAdjListGraph(AdjListGraph graph)
{
    if (graph == nullptr) {
        Die("Graph must not be NULL");
    }

    for (int i = 0; i < graph->VertexCount; ++i) {
        std::cout << i << ':';
        AdjNode *current = graph->Heads[i];
        while (current != nullptr) {
            std::cout << ' ' << current->Vertex;
            current = current->Next;
        }
        std::cout << '\n';
    }
}

void AdjListBFS(AdjListGraph graph, int start, int *order, int *count)
{
    if (graph == nullptr || order == nullptr || count == nullptr) {
        Die("AdjListBFS received NULL argument");
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
     * 3. 遍历 graph->Heads[vertex] 的邻接点。
     * 4. 第一次遇到某个邻接点时，标记 visited 并入队。
     */
    (void)visited;
    (void)queue;
    (void)front;
    (void)back;
    (void)order;
}

void AdjListDFS(AdjListGraph graph, int start, int *order, int *count)
{
    if (graph == nullptr || order == nullptr || count == nullptr) {
        Die("AdjListDFS received NULL argument");
    }

    ValidateVertex(graph, start);

    std::vector<int> visited(graph->VertexCount, 0);
    *count = 0;
    DFSRecursive(graph, start, visited, order, count);
}

int AdjListTopologicalSort(AdjListGraph graph, int *order, int *count)
{
    if (graph == nullptr || order == nullptr || count == nullptr) {
        Die("AdjListTopologicalSort received NULL argument");
    }

    if (!graph->Directed) {
        *count = 0;
        return 0;
    }

    *count = 0;

    /*
     * TODO(student):
     * 1. 统计每个顶点的入度 indegree[]。
     * 2. 将所有入度为 0 的顶点加入队列。
     * 3. 反复取出队首顶点，写入 order。
     * 4. 删除该顶点的出边效果：相邻顶点入度减 1。
     * 5. 新的入度为 0 的顶点继续入队。
     * 6. 若最终输出顶点数等于 graph->VertexCount，返回 1，否则返回 0。
     */
    (void)order;
    return 0;
}
