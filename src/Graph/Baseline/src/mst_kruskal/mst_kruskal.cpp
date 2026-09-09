#include "mst_kruskal.hpp"

#include <algorithm>
#include <vector>

class DisjointSet
{
public:
    explicit DisjointSet(int size) : parent(size), rank(size, 0)
    {
        for (int i = 0; i < size; ++i) {
            parent[i] = i;
        }
    }

    int Find(int value)
    {
        /*
         * TODO(student):
         * 使用路径压缩查找 value 所在集合的根。
         */
        return value;
    }

    bool Union(int left, int right)
    {
        /*
         * TODO(student):
         * 1. 分别找到 left 和 right 的根。
         * 2. 如果根相同，说明加入这条边会成环，返回 false。
         * 3. 按 rank 合并两个集合，必要时更新 rank。
         * 4. 合并成功返回 true。
         */
        (void)left;
        (void)right;
        return false;
    }

private:
    std::vector<int> parent;
    std::vector<int> rank;
};

static bool IsValidEdge(const Edge &edge, int vertex_count)
{
    return edge.From >= 0 && edge.From < vertex_count && edge.To >= 0 && edge.To < vertex_count;
}

int Kruskal(Edge *edges, int edge_count, int vertex_count, Edge *mst_edges, int *total_cost)
{
    if (edges == nullptr || mst_edges == nullptr || total_cost == nullptr || vertex_count <= 0 || edge_count < 0) {
        return 0;
    }

    std::vector<Edge> sorted_edges(edges, edges + edge_count);
    std::sort(sorted_edges.begin(), sorted_edges.end(), [](const Edge &left, const Edge &right) {
        return left.Weight < right.Weight;
    });

    DisjointSet set(vertex_count);
    int selected_count = 0;
    *total_cost = 0;

    for (const Edge &edge : sorted_edges) {
        if (!IsValidEdge(edge, vertex_count)) {
            return 0;
        }

        /*
         * TODO(student):
         * 1. 调用 set.Union(edge.From, edge.To) 判断当前边能否加入 MST。
         * 2. 若能加入，将 edge 写入 mst_edges[selected_count]。
         * 3. 累加 total_cost，并增加 selected_count。
         * 4. 当 selected_count == vertex_count - 1 时返回 1。
         */
        (void)set;
        (void)selected_count;
        (void)edge;
    }

    return 0;
}
