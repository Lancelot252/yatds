#ifndef ZUMA_GAME_HPP
#define ZUMA_GAME_HPP

#include <iostream>
#include <list>
#include <deque>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <functional>

// 颜色定义
const std::vector<char> COLORS = {'R', 'G', 'B', 'Y', 'P'};
const std::unordered_map<char, std::string> COLOR_NAMES = {
    {'R', "Red"}, {'G', "Green"}, {'B', "Blue"}, {'Y', "Yellow"}, {'P', "Purple"}
};
const std::unordered_map<char, std::string> COLOR_HEX = {
    {'R', "#e74c3c"}, {'G', "#2ecc71"}, {'B', "#3498db"}, {'Y', "#f1c40f"}, {'P', "#9b59b6"}
};
const int MATCH_LENGTH = 3;    // 连续3个相同颜色消除
const int INITIAL_TRACK_SIZE = 10; // 初始轨道球数
const int FIRE_QUEUE_SIZE = 10;   // 待发射队列大小

// 道具类型
enum class PowerUpType {
    None,
    Bomb,       // 炸弹: 消除所有指定颜色的球
    Reverse,    // 反转: 反转轨道顺序
    Shuffle     // 洗牌: 重新排列待发射队列
};

// 游戏状态快照（用于撤销）
struct GameSnapshot {
    std::list<char> track;
    std::deque<char> fireQueue;
    int score;
    int ballsFired;
    std::vector<int> powerUpCounts; // [bomb, reverse, shuffle]
};

// 游戏主类
class ZumaGame {
private:
    std::list<char> track;           // 轨道彩球 (双向链表)
    std::deque<char> fireQueue;      // 待发射队列
    int score;                       // 当前得分
    int ballsFired;                  // 已发射球数
    int maxFires;                    // 最大发射次数 (游戏结束条件)
    std::stack<GameSnapshot> undoStack; // 撤销栈
    std::mt19937 rng;                // 随机数生成器
    bool gameOver;
    bool won;
    
    // 道具计数
    int bombCount;
    int reverseCount;
    int shuffleCount;

public:
    ZumaGame(int seed = std::random_device{}()) 
        : score(0), ballsFired(0), maxFires(30), rng(seed), 
          gameOver(false), won(false), bombCount(2), reverseCount(1), shuffleCount(1) {
        initGame();
    }

    // === 初始化 ===
    void initGame() {
        track.clear();
        fireQueue.clear();
        while (!undoStack.empty()) undoStack.pop();
        score = 0;
        ballsFired = 0;
        gameOver = false;
        won = false;
        bombCount = 2;
        reverseCount = 1;
        shuffleCount = 1;

        // 初始化轨道，保证初始无三连
        do {
            track.clear();
            for (int i = 0; i < INITIAL_TRACK_SIZE; ++i) {
                track.push_back(randomColor());
            }
        } while (hasMatch(track));

        // 初始化待发射队列
        for (int i = 0; i < FIRE_QUEUE_SIZE; ++i) {
            fireQueue.push_back(randomColor());
        }
    }

    // === 核心游戏逻辑 ===

    // 发射球到指定位置 (position: 0 ~ track.size())
    // 返回消除过程中的所有步骤描述
    std::vector<std::string> fireBall(int position) {
        std::vector<std::string> steps;

        if (gameOver) {
            steps.push_back("Game is already over!");
            return steps;
        }

        if (fireQueue.empty()) {
            steps.push_back("Fire queue is empty!");
            return steps;
        }

        // 保存快照用于撤销
        saveSnapshot();

        char ball = fireQueue.front();
        fireQueue.pop_front();
        ballsFired++;

        // 插入球到指定位置
        auto it = track.begin();
        std::advance(it, std::min(position, (int)track.size()));
        track.insert(it, ball);

        std::string colorName = COLOR_NAMES.at(ball);
        steps.push_back("Fired " + colorName + " ball at position " + std::to_string(position));

        // 执行消除和连锁消除
        processMatches(steps);

        // 补充待发射队列
        if (fireQueue.empty()) {
            steps.push_back("Fire queue is empty, refilling...");
            for (int i = 0; i < FIRE_QUEUE_SIZE; ++i) {
                fireQueue.push_back(randomColor());
            }
        }

        // 检查游戏状态
        checkGameStatus(steps);

        return steps;
    }

    // === 道具功能 ===

    // 炸弹：消除指定颜色的所有球
    std::vector<std::string> useBomb(char color) {
        std::vector<std::string> steps;
        if (gameOver) { steps.push_back("Game is over!"); return steps; }
        if (bombCount <= 0) { steps.push_back("No bombs left!"); return steps; }

        saveSnapshot();
        bombCount--;

        int removed = 0;
        auto it = track.begin();
        while (it != track.end()) {
            if (*it == color) {
                it = track.erase(it);
                removed++;
            } else {
                ++it;
            }
        }

        steps.push_back("Used BOMB! Removed all " + COLOR_NAMES.at(color) + " balls (" + std::to_string(removed) + " removed)");
        score += removed * 5;

        // 消除后连锁处理
        processMatches(steps);
        checkGameStatus(steps);
        return steps;
    }

    // 反转轨道
    std::vector<std::string> useReverse() {
        std::vector<std::string> steps;
        if (gameOver) { steps.push_back("Game is over!"); return steps; }
        if (reverseCount <= 0) { steps.push_back("No reverse left!"); return steps; }

        saveSnapshot();
        reverseCount--;
        track.reverse();
        steps.push_back("Used REVERSE! Track order has been reversed!");
        return steps;
    }

    // 洗牌：重新排列待发射队列
    std::vector<std::string> useShuffle() {
        std::vector<std::string> steps;
        if (gameOver) { steps.push_back("Game is over!"); return steps; }
        if (shuffleCount <= 0) { steps.push_back("No shuffle left!"); return steps; }

        saveSnapshot();
        shuffleCount--;

        std::vector<char> temp(fireQueue.begin(), fireQueue.end());
        std::shuffle(temp.begin(), temp.end(), rng);
        fireQueue.assign(temp.begin(), temp.end());

        steps.push_back("Used SHUFFLE! Fire queue has been shuffled!");
        return steps;
    }

    // === 撤销 ===
    bool undo() {
        if (undoStack.empty()) return false;
        GameSnapshot snap = undoStack.top();
        undoStack.pop();
        track = snap.track;
        fireQueue = snap.fireQueue;
        score = snap.score;
        ballsFired = snap.ballsFired;
        bombCount = snap.powerUpCounts[0];
        reverseCount = snap.powerUpCounts[1];
        shuffleCount = snap.powerUpCounts[2];
        gameOver = false;
        won = false;
        return true;
    }

    // === 获取游戏状态 ===

    // 获取游戏状态字符串 (JSON格式)
    std::string getStateJson() const {
        std::ostringstream json;
        json << "{";

        // 轨道
        json << "\"track\":[";
        for (auto it = track.begin(); it != track.end(); ++it) {
            if (it != track.begin()) json << ",";
            json << "\"" << *it << "\"";
        }
        json << "],";

        // 待发射队列
        json << "\"fireQueue\":[";
        for (size_t i = 0; i < fireQueue.size(); ++i) {
            if (i > 0) json << ",";
            json << "\"" << fireQueue[i] << "\"";
        }
        json << "],";

        json << "\"score\":" << score << ",";
        json << "\"ballsFired\":" << ballsFired << ",";
        json << "\"maxFires\":" << maxFires << ",";
        json << "\"gameOver\":" << (gameOver ? "true" : "false") << ",";
        json << "\"won\":" << (won ? "true" : "false") << ",";
        json << "\"bombCount\":" << bombCount << ",";
        json << "\"reverseCount\":" << reverseCount << ",";
        json << "\"shuffleCount\":" << shuffleCount << ",";
        json << "\"canUndo\":" << (!undoStack.empty() ? "true" : "false");

        // 颜色信息
        json << ",\"colors\":{";
        for (auto it = COLORS.begin(); it != COLORS.end(); ++it) {
            if (it != COLORS.begin()) json << ",";
            json << "\"" << *it << "\":\"" << COLOR_HEX.at(*it) << "\"";
        }
        json << "}";

        json << "}";
        return json.str();
    }

    // 获取轨道大小
    int getTrackSize() const { return (int)track.size(); }

    // 游戏是否结束
    bool isGameOver() const { return gameOver; }

    // 是否获胜
    bool isWon() const { return won; }

    // 获取待发射队列下一个球的颜色
    char getNextBall() const {
        if (fireQueue.empty()) return ' ';
        return fireQueue.front();
    }

private:
    // 随机颜色
    char randomColor() {
        std::uniform_int_distribution<int> dist(0, (int)COLORS.size() - 1);
        return COLORS[dist(rng)];
    }

    // 保存快照
    void saveSnapshot() {
        GameSnapshot snap;
        snap.track = track;
        snap.fireQueue = fireQueue;
        snap.score = score;
        snap.ballsFired = ballsFired;
        snap.powerUpCounts = {bombCount, reverseCount, shuffleCount};
        undoStack.push(snap);
    }

    // 检查是否有三连匹配
    static bool hasMatch(const std::list<char>& lst) {
        if (lst.size() < 3) return false;
        auto it = lst.begin();
        char prev = *it;
        int count = 1;
        ++it;
        for (; it != lst.end(); ++it) {
            if (*it == prev) {
                count++;
                if (count >= MATCH_LENGTH) return true;
            } else {
                prev = *it;
                count = 1;
            }
        }
        return false;
    }

    // 处理消除和连锁消除
    void processMatches(std::vector<std::string>& steps) {
        bool matched = true;
        int eliminated = 0;

        while (matched) {
            matched = false;
            // 扫描轨道，找到第一个三连及以上
            if (track.size() < 3) break;

            auto start = track.begin();
            auto end = track.begin();
            int count = 1;

            for (auto it = track.begin(); it != track.end(); ) {
                auto next = std::next(it);
                if (next != track.end() && *next == *it) {
                    count++;
                    ++it;
                } else {
                    if (count >= MATCH_LENGTH) {
                        // 找到匹配，记录范围
                        start = it;
                        for (int i = 1; i < count; ++i) --start;
                        end = next;
                        matched = true;
                        break;
                    }
                    count = 1;
                    ++it;
                }
            }

            if (matched) {
                char color = *start;
                // 消除匹配的球
                track.erase(start, end);
                eliminated += count;

                int points = count * 10;
                score += points;

                steps.push_back("Eliminated " + std::to_string(count) + " " + COLOR_NAMES.at(color) + " balls! +" + std::to_string(points) + " points");
            }
        }

        if (eliminated > 0) {
            steps.push_back("Total eliminated: " + std::to_string(eliminated) + " balls");
        }
    }

    // 检查游戏状态
    void checkGameStatus(std::vector<std::string>& steps) {
        if (track.empty()) {
            won = true;
            gameOver = true;
            steps.push_back("Congratulations! You cleared all balls! YOU WIN!");
        } else if (ballsFired >= maxFires) {
            gameOver = true;
            won = false;
            steps.push_back("Game Over! You've used all " + std::to_string(maxFires) + " shots!");
        }
    }
};

#endif // ZUMA_GAME_HPP