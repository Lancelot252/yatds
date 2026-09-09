/*
 * 终端迷宫寻路 Project
 *
 * 目标：
 * 1. 把迷宫这个实际问题转换成图结构。
 * 2. 根据功能需求，自行选择并调用 Baseline 中已经完成的算法接口。
 * 3. 根据 dist[] / prev[] / order[] 等结果解释输出。
 *
 * 注意：Project 不在菜单里告诉你应该用什么算法。
 *      你需要在 TODO(student) 函数中自己完成建模和接口调用。
 */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

#include "../Baseline/src/adjacency_list/graph.hpp"
#include "../Baseline/src/adjacency_matrix/graph.hpp"
#include "../Baseline/src/unweighted_shortest_path/shortest_path.hpp"
#include "../Baseline/src/dijkstra/dijkstra.hpp"
#include "../Baseline/src/edge.hpp"
#include "../Baseline/src/mst_prim/mst_prim.hpp"
#include "../Baseline/src/mst_kruskal/mst_kruskal.hpp"

static const char *kMaze[] = {
    "#####################",
    "#S#.....#...........#",
    "#.#.###.#.####.####.#",
    "#.#.#.#...#.......#.#",
    "#.#.#.#####.#######.#",
    "#.#.....#...#.....#.#",
    "#.##..###.###.###.#.#",
    "#.#...#1112.....#.#.#",
    "#.#.###.###.#.#####.#",
    "#..1#..D#.#.#...#...#",
    "###.#.###...#####.###",
    "#..9#.#.#K......#.#.#",
    "#2###.#.#.#####.#.#.#",
    "#...4.#.......#....E#",
    "#####################",
};

static const int kRows = sizeof(kMaze) / sizeof(kMaze[0]);
static const int kCols = 21;
static const int kDelayMs = 80;

struct Cell
{
    int Row;
    int Col;
};

struct RouteResult
{
    std::vector<int> Path;
    int Value;
};

static bool InBounds(int row, int col)
{
    return row >= 0 && row < kRows && col >= 0 && col < kCols;
}

static bool IsWall(int row, int col)
{
    return kMaze[row][col] == '#';
}

[[maybe_unused]] static bool IsWalkable(int row, int col)
{
    /*
     * Framework helper:
     * 判断一个二维格子是否可以作为图中的顶点。
     *
     * 规则：
     * 1. row / col 不能越界。
     * 2. 墙 '#' 不能通过。
     * 3. '.', 'S', 'K', 'D', 'E' 和数字地形都可以通过。
     */
    return InBounds(row, col) && !IsWall(row, col);
}

static int CellId(int row, int col)
{
    /*
     * Framework helper:
     * 把二维坐标 (row, col) 映射成一维顶点编号。
     */
    return row * kCols + col;
}

static Cell IdToCell(int id)
{
    return Cell{id / kCols, id % kCols};
}

[[maybe_unused]] static int EnterCost(int row, int col)
{
    /*
     * Framework helper:
     * 返回“进入某个格子”的代价。
     *
     * 当前规则：
     * - 数字 '1' 到 '9' 的代价就是对应数字。
     * - 危险门 'D' 的代价为 8。
     * - 其他可走格子的代价为 1。
     */
    char ch = kMaze[row][col];
    if (ch >= '1' && ch <= '9') {
        return ch - '0';
    }
    if (ch == 'D') {
        return 8;
    }
    return 1;
}

static int FindCell(char target)
{
    for (int row = 0; row < kRows; ++row) {
        for (int col = 0; col < kCols; ++col) {
            if (kMaze[row][col] == target) {
                return CellId(row, col);
            }
        }
    }
    return -1;
}

static bool Contains(const std::vector<int> &values, int target)
{
    for (int value : values) {
        if (value == target) {
            return true;
        }
    }
    return false;
}

static void ClearScreen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

static void WaitForEnter()
{
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static void PrintMaze(const std::vector<int> &path, int current, const std::string &title)
{
    std::cout << title << "\n\n";

    for (int row = 0; row < kRows; ++row) {
        for (int col = 0; col < kCols; ++col) {
            int id = CellId(row, col);
            char ch = kMaze[row][col];

            if (id == current && ch != 'S' && ch != 'E' && ch != 'K' && ch != 'D') {
                ch = '@';
            } else if (Contains(path, id) && ch != 'S' && ch != 'E' && ch != 'K' && ch != 'D') {
                ch = '*';
            }

            std::cout << ch;
        }
        std::cout << '\n';
    }

    std::cout << "\nLegend: # wall, . road, digits cost, S start, K key, D dangerous door, E exit\n";
    std::cout << "        * path, @ current. Entering D costs 8 in cost-sensitive tasks.\n";
}

static void AnimatePath(const std::vector<int> &path, const std::string &title)
{
    std::vector<int> drawn;
    for (int id : path) {
        drawn.push_back(id);
        ClearScreen();
        PrintMaze(drawn, id, title);
        std::this_thread::sleep_for(std::chrono::milliseconds(kDelayMs));
    }
}

static void PrintPathCoordinates(const std::vector<int> &path)
{
    for (std::size_t i = 0; i < path.size(); ++i) {
        Cell cell = IdToCell(path[i]);
        if (i > 0) {
            std::cout << " -> ";
        }
        std::cout << '(' << cell.Row << ',' << cell.Col << ')';
    }
    std::cout << '\n';
}

[[maybe_unused]] static std::vector<int> RebuildPath(int start, int goal, const std::vector<int> &prev)
{
    /*
     * TODO(student):
     * 根据最短路接口返回的 prev[] 还原 start 到 goal 的路径。
     *
     * 要求：
     * 1. 从 goal 开始，不断沿 prev[current] 往前找。
     * 2. 直到找到 start，说明路径存在。
     * 3. 如果中途遇到 -1，说明 goal 不可达，返回空路径。
     * 4. 因为回溯得到的是 goal -> start，所以最后需要反转。
     */
    (void)start;
    (void)goal;
    (void)prev;
    return std::vector<int>();
}

[[maybe_unused]] static void AppendPath(std::vector<int> &target, const std::vector<int> &part)
{
    if (part.empty()) {
        return;
    }

    std::size_t begin = target.empty() ? 0 : 1;
    for (std::size_t i = begin; i < part.size(); ++i) {
        target.push_back(part[i]);
    }
}

[[maybe_unused]] static void BuildUnweightedMazeGraph(Graph graph)
{
    /*
     * TODO(student):
     * 把迷宫转换成无权图。
     *
     * 建模规则：
     * 1. 每个可走格子是一个顶点。
     * 2. 上下左右相邻且都可走的两个格子之间有边。
     * 3. 这里使用 directed=1 的图，所以需要为每个方向分别加边。
     */
    (void)graph;
}

[[maybe_unused]] static void BuildWeightedMazeGraph(WGraph graph)
{
    /*
     * TODO(student):
     * 把迷宫转换成带权图。
     *
     * 建模规则：
     * 1. 每个可走格子是一个顶点。
     * 2. 上下左右相邻且都可走的两个格子之间有边。
     * 3. 边权表示“进入邻居格子”的代价。
     */
    (void)graph;
}

static RouteResult FindRouteFromStartToExit(int start, int goal)
{
    /*
     * TODO(student):
     * 功能需求：从 S 到 E 找一条"移动次数尽量少"的路线。
     *
     * 要求：
     * 1. 自己判断应该建立什么图模型。
     * 2. 自己选择 Baseline 中合适的算法接口。
     * 3. 调用接口后，用 RebuildPath 还原路线。
     * 4. 返回 RouteResult{path, value}，其中 value 表示移动次数。
     */
    (void)start;
    (void)goal;
    return RouteResult{std::vector<int>(), -1};
}

static RouteResult FindLowCostRouteFromStartToExit(int start, int goal)
{
    /*
     * TODO(student):
     * 功能需求：从 S 到 E 找一条总代价尽量低的路线。
     *
     * 地图中的数字和危险门 D 都会影响代价。
     * 你需要自己判断如何建图、如何设置边权、调用哪个接口。
     */
    (void)start;
    (void)goal;
    return RouteResult{std::vector<int>(), -1};
}

static RouteResult FindKeyThenExitRoute(int start, int key, int goal)
{
    /*
     * TODO(student):
     * 功能需求：从 S 出发，必须先到 K，再到 E，移动次数尽量少。
     *
     * 你需要自己决定：
     * - 这个任务是否可以拆成多个子问题。
     * - 每个子问题应该调用哪个 Baseline 接口。
     * - 如何把多段路径拼接成完整路线。
     */
    (void)start;
    (void)key;
    (void)goal;
    return RouteResult{std::vector<int>(), -1};
}

static RouteResult FindLowCostKeyThenExitRoute(int start, int key, int goal)
{
    /*
     * TODO(student):
     * 功能需求：从 S 出发，必须先到 K，再到 E，使总代价尽量低。
     *
     * 与任务 3 的对比：
     * - 任务 3 同样是 S→K→E，但目标是"最少步数"。
     * - 任务 4 目标是"最低代价"。
     * - 相同的"先 K 后 E"约束，不同的优化目标，应该选择不同的算法接口。
     *
     * 思考：你能否复用任务 3 的拆分思路？需要换成哪个 Baseline 接口？
     */
    (void)start;
    (void)key;
    (void)goal;
    return RouteResult{std::vector<int>(), -1};
}

static void ShowTaskMenu()
{
    std::cout << "\nProject tasks:\n";
    std::cout << "  1. Find a route from S to E with as few moves as possible\n";
    std::cout << "  2. Find a low-cost route from S to E\n";
    std::cout << "  3. Get the key K first, then reach E\n";
    std::cout << "  4. Get the key K first, then reach E (low-cost)\n";
    std::cout << "  0. Exit\n";
}

static int ReadChoice()
{
    int choice = 0;
    std::cout << "\nChoose a project task: ";
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

static void ShowRouteResult(const RouteResult &result, const std::string &label, const std::string &value_name)
{
    std::cout << "\nTask: " << label << '\n';

    if (result.Path.empty()) {
        std::cout << "No result yet. Complete the corresponding Project TODO and required Baseline first.\n";
        return;
    }

    std::cout << value_name << ": " << result.Value << '\n';
    std::cout << "Path: ";
    PrintPathCoordinates(result.Path);
    AnimatePath(result.Path, label);
}

int main()
{
    int start = FindCell('S');
    int key = FindCell('K');
    int goal = FindCell('E');

    if (start == -1 || key == -1 || goal == -1) {
        std::cerr << "Maze must contain S, K and E.\n";
        return 1;
    }

    while (true) {
        ClearScreen();
        PrintMaze(std::vector<int>(), -1, "Terminal maze pathfinding project");
        ShowTaskMenu();

        int choice = ReadChoice();
        if (choice == 0) {
            break;
        }

        if (choice == 1) {
            ShowRouteResult(
                FindRouteFromStartToExit(start, goal),
                "route from S to E with few moves",
                "Moves");
        } else if (choice == 2) {
            ShowRouteResult(
                FindLowCostRouteFromStartToExit(start, goal),
                "low-cost route from S to E",
                "Cost");
        } else if (choice == 3) {
            ShowRouteResult(
                FindKeyThenExitRoute(start, key, goal),
                "get key then exit",
                "Moves");
        } else if (choice == 4) {
            ShowRouteResult(
                FindLowCostKeyThenExitRoute(start, key, goal),
                "low-cost route through key K to E",
                "Cost");
        } else {
            std::cout << "Invalid choice.\n";
        }

        WaitForEnter();
    }

    return 0;
}
