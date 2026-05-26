#ifndef ZUMA_GAME_HPP
#define ZUMA_GAME_HPP

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <cstdlib>

// 彩球颜色枚举
enum class BallColor {
    RED = 0,    // 红
    BLUE = 1,   // 蓝
    GREEN = 2,  // 绿
    YELLOW = 3, // 黄
    PURPLE = 4, // 紫
    NONE = -1   // 空
};

// 将颜色转换为字符
inline char colorToChar(BallColor color) {
    switch (color) {
        case BallColor::RED: return 'R';
        case BallColor::BLUE: return 'B';
        case BallColor::GREEN: return 'G';
        case BallColor::YELLOW: return 'Y';
        case BallColor::PURPLE: return 'P';
        default: return ' ';
    }
}

// 将字符转换为颜色
inline BallColor charToColor(char c) {
    switch (c) {
        case 'R': case 'r': return BallColor::RED;
        case 'B': case 'b': return BallColor::BLUE;
        case 'G': case 'g': return BallColor::GREEN;
        case 'Y': case 'y': return BallColor::YELLOW;
        case 'P': case 'p': return BallColor::PURPLE;
        default: return BallColor::NONE;
    }
}

// 将颜色转换为字符串
inline std::string colorToString(BallColor color) {
    switch (color) {
        case BallColor::RED: return "red";
        case BallColor::BLUE: return "blue";
        case BallColor::GREEN: return "green";
        case BallColor::YELLOW: return "yellow";
        case BallColor::PURPLE: return "purple";
        default: return "none";
    }
}

// 链表节点 - 表示轨道上的彩球
struct BallNode {
    BallColor color;
    BallNode* next;
    BallNode* prev;
    
    BallNode(BallColor c) : color(c), next(nullptr), prev(nullptr) {}
};

// 游戏状态快照 - 用于撤销功能
struct GameState {
    std::string track;           // 轨道状态字符串
    std::vector<char> queue;     // 发射队列状态
    int score;
    int moves;
    
    GameState(const std::string& t, const std::vector<char>& q, int s, int m)
        : track(t), queue(q), score(s), moves(m) {}
};

// 祖玛游戏类
class ZumaGame {
private:
    BallNode* trackHead;                    // 轨道头节点
    BallNode* trackTail;                    // 轨道尾节点
    int trackLength;                        // 轨道长度
    std::queue<BallColor> launchQueue;      // 待发射队列
    std::stack<GameState> history;           // 历史状态栈（用于撤销）
    int score;                              // 得分
    int moves;                              // 移动次数
    bool gameOver;                          // 游戏结束标志
    bool win;                               // 胜利标志
    const int MAX_QUEUE_SIZE = 10;          // 队列最大容量
    const int INITIAL_TRACK_LENGTH = 15;    // 初始轨道长度
    const int INITIAL_QUEUE_SIZE = 5;       // 初始队列大小
    const int WIN_SCORE = 1000;             // 胜利分数
    const int MAX_MOVES = 50;               // 最大移动次数

public:
    // 构造函数
    ZumaGame() : trackHead(nullptr), trackTail(nullptr), trackLength(0),
                 score(0), moves(0), gameOver(false), win(false) {
        srand(time(nullptr));
        initGame();
    }

    // 析构函数
    ~ZumaGame() {
        clearTrack();
    }

    // 初始化游戏
    void initGame() {
        clearTrack();
        while (!launchQueue.empty()) launchQueue.pop();
        while (!history.empty()) history.pop();
        
        score = 0;
        moves = 0;
        gameOver = false;
        win = false;
        
        // 初始化轨道
        for (int i = 0; i < INITIAL_TRACK_LENGTH; i++) {
            BallColor color = static_cast<BallColor>(rand() % 5);
            appendToTrack(color);
        }
        
        // 初始化发射队列
        refillQueue();
        
        // 保存初始状态
        saveState();
    }

    // 在轨道尾部添加彩球
    void appendToTrack(BallColor color) {
        BallNode* newNode = new BallNode(color);
        if (!trackHead) {
            trackHead = trackTail = newNode;
        } else {
            trackTail->next = newNode;
            newNode->prev = trackTail;
            trackTail = newNode;
        }
        trackLength++;
    }

    // 在指定位置插入彩球
    bool insertAt(int position, BallColor color) {
        if (position < 0 || position > trackLength) return false;
        
        BallNode* newNode = new BallNode(color);
        
        if (position == 0) {
            // 插入头部
            newNode->next = trackHead;
            if (trackHead) trackHead->prev = newNode;
            trackHead = newNode;
            if (!trackTail) trackTail = newNode;
        } else if (position == trackLength) {
            // 插入尾部
            appendToTrack(color);
            delete newNode;
            return true;
        } else {
            // 插入中间
            BallNode* current = trackHead;
            for (int i = 0; i < position - 1; i++) {
                current = current->next;
            }
            newNode->next = current->next;
            newNode->prev = current;
            if (current->next) current->next->prev = newNode;
            current->next = newNode;
        }
        
        trackLength++;
        return true;
    }

    // 删除指定位置的彩球
    bool removeAt(int position) {
        if (position < 0 || position >= trackLength || !trackHead) return false;
        
        BallNode* toDelete;
        
        if (position == 0) {
            toDelete = trackHead;
            trackHead = trackHead->next;
            if (trackHead) trackHead->prev = nullptr;
            else trackTail = nullptr;
        } else {
            BallNode* current = trackHead;
            for (int i = 0; i < position; i++) {
                current = current->next;
            }
            toDelete = current;
            if (toDelete->prev) toDelete->prev->next = toDelete->next;
            if (toDelete->next) toDelete->next->prev = toDelete->prev;
            if (toDelete == trackTail) trackTail = toDelete->prev;
        }
        
        delete toDelete;
        trackLength--;
        return true;
    }

    // 获取指定位置的彩球颜色
    BallColor getBallAt(int position) {
        if (position < 0 || position >= trackLength) return BallColor::NONE;
        
        BallNode* current = trackHead;
        for (int i = 0; i < position; i++) {
            current = current->next;
        }
        return current->color;
    }

    // 查找连续相同颜色的彩球范围 [start, end]
    bool findConsecutiveRange(int insertPos, int& start, int& end, BallColor color) {
        // 从插入位置开始向左查找
        start = insertPos;
        while (start > 0) {
            if (getBallAt(start - 1) == color) {
                start--;
            } else {
                break;
            }
        }
        
        // 从插入位置开始向右查找
        end = insertPos;
        while (end < trackLength - 1) {
            if (getBallAt(end + 1) == color) {
                end++;
            } else {
                break;
            }
        }
        
        // 检查是否满足消除条件（3个或以上）
        return (end - start + 1) >= 3;
    }

    // 消除指定范围的彩球
    void removeRange(int start, int end) {
        for (int i = end; i >= start; i--) {
            removeAt(i);
        }
    }

    // 执行消除和连锁消除
    int eliminate(int insertPos, BallColor insertedColor) {
        int totalEliminated = 0;
        int currentPos = insertPos;
        
        while (true) {
            int start, end;
            if (findConsecutiveRange(currentPos, start, end, insertedColor)) {
                int count = end - start + 1;
                totalEliminated += count;
                
                // 计算得分（连锁消除有加成）
                int chainBonus = (totalEliminated > count) ? 2 : 1;
                score += count * 10 * chainBonus;
                
                // 删除这段彩球
                removeRange(start, end);
                
                // 检查是否需要继续连锁消除
                if (trackLength == 0) break;
                
                // 找到新的检查位置（左右连接处）
                if (start < trackLength) {
                    // 检查连接处是否形成新的消除
                    BallColor leftColor = (start > 0) ? getBallAt(start - 1) : BallColor::NONE;
                    BallColor rightColor = (start < trackLength) ? getBallAt(start) : BallColor::NONE;
                    
                    if (leftColor != BallColor::NONE && leftColor == rightColor) {
                        currentPos = start;
                        insertedColor = leftColor;
                        continue;
                    }
                }
                break;
            } else {
                break;
            }
        }
        
        return totalEliminated;
    }

    // 发射彩球
    bool launchBall(int position) {
        if (gameOver || launchQueue.empty()) return false;
        
        // 保存当前状态
        saveState();
        
        BallColor color = launchQueue.front();
        launchQueue.pop();
        
        // 插入彩球
        insertAt(position, color);
        
        // 执行消除
        eliminate(position, color);
        
        // 补充队列
        if (launchQueue.size() < INITIAL_QUEUE_SIZE) {
            launchQueue.push(static_cast<BallColor>(rand() % 5));
        }
        
        moves++;
        
        // 检查游戏状态
        checkGameStatus();
        
        return true;
    }

    // 补充发射队列
    void refillQueue() {
        while (launchQueue.size() < MAX_QUEUE_SIZE) {
            launchQueue.push(static_cast<BallColor>(rand() % 5));
        }
    }

    // 保存游戏状态
    void saveState() {
        std::string trackStr = trackToString();
        std::vector<char> queueVec;
        std::queue<BallColor> tempQueue = launchQueue;
        while (!tempQueue.empty()) {
            queueVec.push_back(colorToChar(tempQueue.front()));
            tempQueue.pop();
        }
        history.push(GameState(trackStr, queueVec, score, moves));
    }

    // 撤销操作
    bool undo() {
        if (history.size() <= 1) return false; // 保留初始状态
        
        history.pop(); // 移除当前状态
        if (history.empty()) return false;
        
        GameState& state = history.top();
        
        // 恢复轨道
        clearTrack();
        for (char c : state.track) {
            appendToTrack(charToColor(c));
        }
        
        // 恢复队列
        while (!launchQueue.empty()) launchQueue.pop();
        for (char c : state.queue) {
            launchQueue.push(charToColor(c));
        }
        
        score = state.score;
        moves = state.moves;
        gameOver = false;
        win = false;
        
        return true;
    }

    // 检查游戏状态
    void checkGameStatus() {
        // 胜利条件：轨道为空或达到目标分数
        if (trackLength == 0) {
            gameOver = true;
            win = true;
            return;
        }
        
        if (score >= WIN_SCORE) {
            gameOver = true;
            win = true;
            return;
        }
        
        // 失败条件：移动次数用完
        if (moves >= MAX_MOVES) {
            gameOver = true;
            win = false;
            return;
        }
    }

    // 清空轨道
    void clearTrack() {
        BallNode* current = trackHead;
        while (current) {
            BallNode* next = current->next;
            delete current;
            current = next;
        }
        trackHead = trackTail = nullptr;
        trackLength = 0;
    }

    // 将轨道转换为字符串
    std::string trackToString() {
        std::string result;
        BallNode* current = trackHead;
        while (current) {
            result += colorToChar(current->color);
            current = current->next;
        }
        return result;
    }

    // 获取发射队列内容
    std::vector<std::string> getQueueColors() {
        std::vector<std::string> result;
        std::queue<BallColor> tempQueue = launchQueue;
        while (!tempQueue.empty()) {
            result.push_back(colorToString(tempQueue.front()));
            tempQueue.pop();
        }
        return result;
    }

    // 获取下一个要发射的彩球颜色
    std::string getNextBall() {
        if (launchQueue.empty()) return "none";
        return colorToString(launchQueue.front());
    }

    // 获取游戏状态 JSON 字符串
    std::string getGameStateJson() {
        std::stringstream json;
        json << "{";
        json << "\"track\":" << trackToJson() << ",";
        json << "\"queue\":" << queueToJson() << ",";
        json << "\"nextBall\":\"" << getNextBall() << "\",";
        json << "\"score\":" << score << ",";
        json << "\"moves\":" << moves << ",";
        json << "\"maxMoves\":" << MAX_MOVES << ",";
        json << "\"trackLength\":" << trackLength << ",";
        json << "\"gameOver\":" << (gameOver ? "true" : "false") << ",";
        json << "\"win\":" << (win ? "true" : "false");
        json << "}";
        return json.str();
    }

    // 轨道转 JSON
    std::string trackToJson() {
        std::stringstream json;
        json << "[";
        BallNode* current = trackHead;
        bool first = true;
        while (current) {
            if (!first) json << ",";
            json << "\"" << colorToString(current->color) << "\"";
            first = false;
            current = current->next;
        }
        json << "]";
        return json.str();
    }

    // 队列转 JSON
    std::string queueToJson() {
        std::stringstream json;
        json << "[";
        std::queue<BallColor> tempQueue = launchQueue;
        bool first = true;
        while (!tempQueue.empty()) {
            if (!first) json << ",";
            json << "\"" << colorToString(tempQueue.front()) << "\"";
            first = false;
            tempQueue.pop();
        }
        json << "]";
        return json.str();
    }

    // 获取分数
    int getScore() const { return score; }
    
    // 获取移动次数
    int getMoves() const { return moves; }
    
    // 获取轨道长度
    int getTrackLength() const { return trackLength; }
    
    // 检查游戏是否结束
    bool isGameOver() const { return gameOver; }
    
    // 检查是否胜利
    bool isWin() const { return win; }

    // 使用道具：炸弹（消除指定位置及其周围的彩球）
    bool useBomb(int position) {
        if (gameOver || position < 0 || position >= trackLength) return false;
        
        saveState();
        
        // 消除指定位置及其左右相邻的彩球
        int start = std::max(0, position - 1);
        int end = std::min(trackLength - 1, position + 1);
        int count = end - start + 1;
        
        removeRange(start, end);
        score += count * 5;
        
        // 检查连锁消除
        if (trackLength > 0) {
            // 检查连接处
            if (start > 0 && start < trackLength) {
                BallColor leftColor = getBallAt(start - 1);
                BallColor rightColor = getBallAt(start);
                if (leftColor == rightColor) {
                    eliminate(start, leftColor);
                }
            }
        }
        
        checkGameStatus();
        return true;
    }

    // 使用道具：彩虹球（可以匹配任何颜色）
    bool useRainbowBall(int position, BallColor targetColor) {
        if (gameOver || position < 0 || position > trackLength) return false;
        
        saveState();
        
        // 插入彩虹球（使用目标颜色）
        insertAt(position, targetColor);
        
        // 执行消除
        eliminate(position, targetColor);
        
        moves++;
        checkGameStatus();
        return true;
    }

    // 使用道具：刷新队列
    bool useRefreshQueue() {
        if (gameOver) return false;
        
        saveState();
        
        while (!launchQueue.empty()) {
            launchQueue.pop();
        }
        
        for (int i = 0; i < INITIAL_QUEUE_SIZE; i++) {
            launchQueue.push(static_cast<BallColor>(rand() % 5));
        }
        
        return true;
    }
};

#endif // ZUMA_GAME_HPP
