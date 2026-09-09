#include "dijkstra.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

struct WEdge
{
    int To;
    int Weight;
};

struct WGraphRecord
{
    int VertexCount;
    bool Directed;
    std::vector<std::vector<WEdge>> Adjacent;
};

static void Die(const char *message)
{
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
}

static void ValidateVertex(WGraph graph, int vertex)
{
    if (vertex < 0 || vertex >= graph->VertexCount) {
        Die("Vertex index out of range");
    }
}

WGraph CreateDijkstraGraph(int vertex_count, int directed)
{
    if (vertex_count <= 0) {
        Die("Vertex count must be positive");
    }
    return new WGraphRecord{vertex_count, directed != 0, std::vector<std::vector<WEdge>>(vertex_count)};
}

void DisposeDijkstraGraph(WGraph graph)
{
    delete graph;
}

void AddDijkstraEdge(WGraph graph, int from, int to, int weight)
{
    if (graph == nullptr) {
        Die("Graph must not be NULL");
    }
    if (weight < 0) {
        Die("Dijkstra requires non-negative edge weights");
    }
    ValidateVertex(graph, from);
    ValidateVertex(graph, to);

    graph->Adjacent[from].push_back(WEdge{to, weight});
    if (!graph->Directed) {
        graph->Adjacent[to].push_back(WEdge{from, weight});
    }
}

void Dijkstra(WGraph graph, int src, int *dist, int *prev)
{
    if (graph == nullptr || dist == nullptr || prev == nullptr) {
        Die("Dijkstra received NULL argument");
    }
    ValidateVertex(graph, src);

    for (int i = 0; i < graph->VertexCount; ++i) {
        dist[i] = -1;
        prev[i] = -1;
    }
    dist[src] = 0;

    /*
     * TODO(student):
     * 1. 准备 visited[] 和 best[]，best[src] = 0，其余为无穷大。
     * 2. 重复 vertex_count 次：选择未访问且 best 最小的顶点。
     * 3. 若当前最小值仍为无穷大，说明剩余顶点不可达，可以结束。
     * 4. 遍历当前顶点的出边，尝试松弛 best[edge.To]。
     * 5. 松弛成功时同步更新 prev[edge.To]。
     * 6. 将 best[] 中可达顶点写回 dist[]，不可达保持 -1。
     */
}
