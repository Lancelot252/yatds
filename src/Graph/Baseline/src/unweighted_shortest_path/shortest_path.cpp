#include "shortest_path.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

struct GraphRecord
{
    int VertexCount;
    bool Directed;
    std::vector<std::vector<int>> Adjacent;
};

static void Die(const char *message)
{
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
}

static void ValidateVertex(Graph graph, int vertex)
{
    if (vertex < 0 || vertex >= graph->VertexCount) {
        Die("Vertex index out of range");
    }
}

Graph CreateShortestPathGraph(int vertex_count, int directed)
{
    if (vertex_count <= 0) {
        Die("Vertex count must be positive");
    }
    return new GraphRecord{vertex_count, directed != 0, std::vector<std::vector<int>>(vertex_count)};
}

void DisposeShortestPathGraph(Graph graph)
{
    delete graph;
}

void AddShortestPathEdge(Graph graph, int from, int to)
{
    if (graph == nullptr) {
        Die("Graph must not be NULL");
    }
    ValidateVertex(graph, from);
    ValidateVertex(graph, to);

    graph->Adjacent[from].push_back(to);
    if (!graph->Directed) {
        graph->Adjacent[to].push_back(from);
    }
}

void ShortestPath(Graph graph, int src, int *dist, int *prev)
{
    if (graph == nullptr || dist == nullptr || prev == nullptr) {
        Die("ShortestPath received NULL argument");
    }
    ValidateVertex(graph, src);

    for (int i = 0; i < graph->VertexCount; ++i) {
        dist[i] = -1;
        prev[i] = -1;
    }

    dist[src] = 0;

    /*
     * TODO(student):
     * 1. 建立 BFS 队列，并把 src 入队。
     * 2. 每次取出队首顶点 vertex，遍历 graph->Adjacent[vertex]。
     * 3. 第一次访问到 next 时，设置 dist[next] = dist[vertex] + 1。
     * 4. 设置 prev[next] = vertex，并把 next 入队。
     */
}
