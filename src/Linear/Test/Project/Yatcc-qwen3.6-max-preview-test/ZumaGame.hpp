#ifndef ZUMA_GAME_HPP
#define ZUMA_GAME_HPP

#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <sstream>
#include <algorithm>
#include <stdexcept>

// ============================================================
// 数据结构设计说明
// ============================================================
// 1. 彩球轨道：使用双向链表（Doubly Linked List）
//    - 原因：频繁在任意位置插入和删除节点，双向链表 O(1) 操作
//    - 优点：支持高效的中间插入/删除，适合祖玛的消除机制
//
// 2. 待发射彩球队列：使用 std::queue（队列 Queue）
//    - 原因：彩球按顺序发射，先进先出（FIFO）
//    - 优点：语义清晰，O(1) 入队出队
//
// 3. 撤销历史：使用 std::stack（栈 Stack）
//    - 原因：撤销操作是后进先出（LIFO）
//    - 优点：完美匹配撤销语义，O(1) 压栈弹栈
// ============================================================

/// @brief 双向链表节点，表示轨道上的一个彩球
struct BallNode {
    char color;           // 彩球颜色（用单个大写字母表示）
    BallNode* prev;       // 前驱节点
    BallNode* next;       // 后继节点

    BallNode(char c) : color(c), prev(nullptr), next(nullptr) {}
};

/// @brief 游戏状态快照，用于撤销操作
struct GameState {
    std::string track;        // 轨道彩球序列
    std::string launchQueue;  // 待发射队列
    int score;                // 当前得分
    int ballsEliminated;      // 已消除球数

    GameState() : score(0), ballsEliminated(0) {}
};

/// @brief 祖玛游戏核心逻辑类
class ZumaGame {
private:
    BallNode* head;                       // 链表头哨兵
    BallNode* tail;                       // 链表尾哨兵
    int trackSize;                        // 轨道上彩球数量
    std::queue<char> launchQueue;         // 待发射彩球队列（队列）
    std::stack<GameState> undoStack;      // 撤销历史（栈）
    int score;                            // 当前得分
    int ballsEliminated;                  // 累计消除球数
    bool gameOver;                        // 游戏是否结束
    bool gameWon;                         // 游戏是否胜利

    /// @brief 保存当前状态到撤销栈
    void saveState() {
        GameState state;
        state.score = score;
        state.ballsEliminated = ballsEliminated;

        // 遍历链表获取轨道状态
        BallNode* cur = head->next;
        while (cur != tail) {
            state.track += cur->color;
            cur = cur->next;
        }

        // 复制队列状态
        std::queue<char> tempQ = launchQueue;
        while (!tempQ.empty()) {
            state.launchQueue += tempQ.front();
            tempQ.pop();
        }

        undoStack.push(state);
    }

    /// @brief 从位置 pos 开始向两侧检测并消除连续 >=3 个相同颜色的球
    /// @param pos 插入位置的节点（新插入的球）
    /// @return 消除的球数
    int eliminate(BallNode* pos) {
        if (pos == head || pos == tail) return 0;

        char color = pos->color;
        BallNode* left = pos;
        BallNode* right = pos;

        // 向左扩展
        while (left->prev != head && left->prev->color == color) {
            left = left->prev;
        }
        // 向右扩展
        while (right->next != tail && right->next->color == color) {
            right = right->next;
        }

        // 计算连续相同颜色球的数量
        int count = 0;
        BallNode* cur = left;
        while (cur != right->next) {
            count++;
            cur = cur->next;
        }

        if (count >= 3) {
            // 记录消除区间的前后节点
            BallNode* beforeLeft = left->prev;
            BallNode* afterRight = right->next;

            // 删除区间 [left, right]
            cur = left;
            while (cur != afterRight) {
                BallNode* nextNode = cur->next;
                delete cur;
                cur = nextNode;
                trackSize--;
            }

            // 重新连接
            beforeLeft->next = afterRight;
            afterRight->prev = beforeLeft;

            ballsEliminated += count;
            score += count * 10; // 每消除一个球得10分

            // 连锁消除：检查连接处
            if (beforeLeft != head && afterRight != tail &&
                beforeLeft->color == afterRight->color) {
                // 从连接处继续消除
                score += eliminateChain(beforeLeft, afterRight);
            }

            return count;
        }

        return 0;
    }

    /// @brief 连锁消除：从两个相邻节点向两侧扩展消除
    int eliminateChain(BallNode* left, BallNode* right) {
        if (left == head || right == tail) return 0;

        char color = left->color;
        BallNode* l = left;
        BallNode* r = right;

        // 向左扩展
        while (l->prev != head && l->prev->color == color) {
            l = l->prev;
        }
        // 向右扩展
        while (r->next != tail && r->next->color == color) {
            r = r->next;
        }

        int count = 0;
        BallNode* cur = l;
        while (cur != r->next) {
            count++;
            cur = cur->next;
        }

        if (count >= 3) {
            BallNode* beforeL = l->prev;
            BallNode* afterR = r->next;

            cur = l;
            while (cur != afterR) {
                BallNode* nextNode = cur->next;
                delete cur;
                cur = nextNode;
                trackSize--;
            }

            beforeL->next = afterR;
            afterR->prev = beforeL;

            ballsEliminated += count;
            int chainScore = count * 15; // 连锁消除得分更高

            // 继续连锁
            if (beforeL != head && afterR != tail &&
                beforeL->color == afterR->color) {
                chainScore += eliminateChain(beforeL, afterR);
            }

            return chainScore;
        }

        return 0;
    }

    /// @brief 检查游戏是否结束
    void checkGameEnd() {
        if (trackSize == 0 && launchQueue.empty()) {
            gameOver = true;
            gameWon = true;
        } else if (launchQueue.empty() && trackSize > 0) {
            // 队列为空但轨道还有球，游戏结束（失败）
            gameOver = true;
            gameWon = false;
        }
    }

public:
    /// @brief 构造函数
    ZumaGame() : trackSize(0), score(0), ballsEliminated(0), gameOver(false), gameWon(false) {
        head = new BallNode(0);
        tail = new BallNode(0);
        head->next = tail;
        tail->prev = head;
    }

    /// @brief 析构函数
    ~ZumaGame() {
        clearTrack();
        delete head;
        delete tail;
    }

    /// @brief 清空轨道
    void clearTrack() {
        BallNode* cur = head->next;
        while (cur != tail) {
            BallNode* nextNode = cur->next;
            delete cur;
            cur = nextNode;
        }
        head->next = tail;
        tail->prev = head;
        trackSize = 0;
    }

    /// @brief 初始化游戏
    /// @param initialTrack 初始轨道彩球序列（如 "RRBBGG"）
    /// @param initialQueue 待发射彩球队列（如 "RBGRBG"）
    void initGame(const std::string& initialTrack, const std::string& initialQueue) {
        clearTrack();
        while (!launchQueue.empty()) launchQueue.pop();
        while (!undoStack.empty()) undoStack.pop();
        score = 0;
        ballsEliminated = 0;
        gameOver = false;
        gameWon = false;

        // 构建初始轨道（双向链表）
        for (char c : initialTrack) {
            BallNode* node = new BallNode(c);
            node->prev = tail->prev;
            node->next = tail;
            tail->prev->next = node;
            tail->prev = node;
            trackSize++;
        }

        // 构建待发射队列
        for (char c : initialQueue) {
            launchQueue.push(c);
        }
    }

    /// @brief 在轨道指定位置插入彩球
    /// @param position 插入位置（0表示最左端，trackSize表示最右端）
    /// @return 消除的球数，-1表示操作失败
    int insertBall(int position) {
        if (gameOver || launchQueue.empty()) return -1;
        if (position < 0 || position > trackSize) return -1;

        // 保存状态用于撤销
        saveState();

        // 从队列取出一个球
        char ballColor = launchQueue.front();
        launchQueue.pop();

        // 找到插入位置
        BallNode* cur = head->next;
        for (int i = 0; i < position; i++) {
            cur = cur->next;
        }

        // 创建新节点并插入
        BallNode* newNode = new BallNode(ballColor);
        newNode->prev = cur->prev;
        newNode->next = cur;
        cur->prev->next = newNode;
        cur->prev = newNode;
        trackSize++;

        // 执行消除
        int eliminated = eliminate(newNode);

        // 检查游戏结束
        checkGameEnd();

        return eliminated;
    }

    /// @brief 撤销上一次操作
    /// @return 是否撤销成功
    bool undo() {
        if (undoStack.empty() || gameOver) return false;

        GameState state = undoStack.top();
        undoStack.pop();

        // 恢复轨道
        clearTrack();
        for (char c : state.track) {
            BallNode* node = new BallNode(c);
            node->prev = tail->prev;
            node->next = tail;
            tail->prev->next = node;
            tail->prev = node;
            trackSize++;
        }

        // 恢复队列
        while (!launchQueue.empty()) launchQueue.pop();
        for (char c : state.launchQueue) {
            launchQueue.push(c);
        }

        score = state.score;
        ballsEliminated = state.ballsEliminated;
        gameOver = false;
        gameWon = false;

        return true;
    }

    /// @brief 获取轨道彩球序列
    std::string getTrack() const {
        std::string result;
        BallNode* cur = head->next;
        while (cur != tail) {
            result += cur->color;
            cur = cur->next;
        }
        return result;
    }

    /// @brief 获取待发射队列
    std::string getLaunchQueue() const {
        std::string result;
        std::queue<char> tempQ = launchQueue;
        while (!tempQ.empty()) {
            result += tempQ.front();
            tempQ.pop();
        }
        return result;
    }

    /// @brief 获取下一个待发射的球
    char getNextBall() const {
        if (launchQueue.empty()) return 0;
        return launchQueue.front();
    }

    /// @brief 获取当前得分
    int getScore() const { return score; }

    /// @brief 获取已消除球数
    int getBallsEliminated() const { return ballsEliminated; }

    /// @brief 获取轨道大小
    int getTrackSize() const { return trackSize; }

    /// @brief 获取队列大小
    int getQueueSize() const { return (int)launchQueue.size(); }

    /// @brief 游戏是否结束
    bool isGameOver() const { return gameOver; }

    /// @brief 游戏是否胜利
    bool isGameWon() const { return gameWon; }

    /// @brief 获取可撤销次数
    int getUndoCount() const { return (int)undoStack.size(); }

    /// @brief 获取游戏状态的 JSON 字符串
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{";
        oss << "\"track\":\"" << getTrack() << "\",";
        oss << "\"launchQueue\":\"" << getLaunchQueue() << "\",";
        oss << "\"nextBall\":\"" << getNextBall() << "\",";
        oss << "\"score\":" << score << ",";
        oss << "\"ballsEliminated\":" << ballsEliminated << ",";
        oss << "\"trackSize\":" << trackSize << ",";
        oss << "\"queueSize\":" << (int)launchQueue.size() << ",";
        oss << "\"undoCount\":" << (int)undoStack.size() << ",";
        oss << "\"gameOver\":" << (gameOver ? "true" : "false") << ",";
        oss << "\"gameWon\":" << (gameWon ? "true" : "false");
        oss << "}";
        return oss.str();
    }
};

#endif // ZUMA_GAME_HPP
