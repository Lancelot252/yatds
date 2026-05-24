#pragma once

#include <list>
#include <queue>
#include <stack>
#include <string>
#include <random>
#include <vector>

/**
 * ZumaGame - 祖玛游戏核心逻辑
 *
 * 数据结构设计：
 * - std::list<char> (双向链表): 表示彩球轨道，支持在任意位置 O(1) 插入和删除彩球
 * - std::queue<char> (队列): 表示待发射彩球队列，FIFO 先进先出
 * - std::stack<std::string> (栈): 保存每次操作前的游戏状态 JSON，用于撤销操作 LIFO
 * - std::mt19937: Mersenne Twister 随机数生成器，用于产生随机颜色的彩球
 */
class ZumaGame {
public:
    ZumaGame();

    // ==================== 游戏初始化 ====================
    void init(int trackSize, int queueSize, int maxTrack);

    // ==================== 核心操作 ====================
    bool fire(int position);
    bool undo();

    // ==================== 小道具功能 ====================
    bool bomb(int position);
    void shuffleNextBall();
    bool pushBack();
    bool rainbow(int position);

    // ==================== 状态查询 ====================
    std::string getTrackString() const;
    std::string getQueueString() const;
    std::vector<char> getTrack() const;
    std::vector<char> getQueue() const;
    char getNextBall() const;
    int getScore() const;
    int getCombo() const;
    int getTotalEliminated() const;
    int getMaxTrack() const;
    int getTrackSize() const;
    int getQueueSize() const;
    bool isWin() const;
    bool isLose() const;
    bool isGameOver() const;
    std::string getStateJSON() const;

private:
    // ==================== 核心数据结构 ====================
    std::list<char> track_;            // 双向链表：彩球轨道
    std::queue<char> ballQueue_;       // 队列：待发射彩球
    std::stack<std::string> history_;  // 栈：保存操作前的 JSON，实现撤销

    int score_;
    int maxTrackSize_;
    int comboCount_;
    int totalEliminated_;
    std::mt19937 rng_;

    static const std::vector<char> COLORS;

    // ==================== 内部辅助函数 ====================
    char randomColor();
    int checkAndEliminate();
    void saveState();
    void refillQueue(int count);
};