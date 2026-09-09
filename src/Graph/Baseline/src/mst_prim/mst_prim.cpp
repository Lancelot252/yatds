#include "mst_prim.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

struct WEdge
{
    int To;
    int Weight;
};

struct PrimGraphRecord
{
    int VertexCount;
    std::vector<std::vector<WEdge>> Adjacent;
};

static void Die(const char *message)
{
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
}

static void ValidateVertex(PrimGraph graph, int vertex)
{
    if (vertex < 0 || vertex >= graph->VertexCount) {
        Die("Vertex index out of range");
    }
}

PrimGraph CreatePrimGraph(int vertex_count)
{
    if (vertex_count <= 0) {
        Die("Vertex count must be positive");
    }
    return new PrimGraphRecord{vertex_count, std::vector<std::vector<WEdge>>(vertex_count)};
}

void DisposePrimGraph(PrimGraph graph)
{
    delete graph;
}

void AddPrimEdge(PrimGraph graph, int from, int to, int weight)
{
    if (graph == nullptr) {
        Die("Graph must not be NULL");
    }
    ValidateVertex(graph, from);
    ValidateVertex(graph, to);

    graph->Adjacent[from].push_back(WEdge{to, weight});
    graph->Adjacent[to].push_back(WEdge{from, weight});
}

int Prim(PrimGraph graph, Edge *mst_edges, int *total_cost)
{
    if (graph == nullptr || mst_edges == nullptr || total_cost == nullptr) {
        return 0;
    }

    *total_cost = 0;

    /*
     * TODO(student):
     * 1. 准备 selected[]、best_weight[]、parent[]。
     * 2. 从 0 号顶点开始，best_weight[0] = 0。
     * 3. 每轮选择一个未加入 MST 且 best_weight 最小的顶点。
     * 4. 如果找不到可选顶点，说明图不连通，返回 0。
     * 5. 若该顶点有 parent，则把 parent -> vertex 写入 mst_edges。
     * 6. 用该顶点的邻边更新其他顶点的 best_weight 和 parent。
     * 7. 成功选出 vertex_count - 1 条边时返回 1。
     */

    return 0;
}
