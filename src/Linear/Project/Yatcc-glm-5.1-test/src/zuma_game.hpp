#ifndef ZUMA_GAME_HPP
#define ZUMA_GAME_HPP

#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <functional>

// 彩球颜色枚举
enum class BallColor {
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    YELLOW = 3,
    PURPLE = 4,
    ORANGE = 5,
    COUNT = 6
};

// 将颜色转换为字符表示
inline char colorToChar(BallColor c) {
    switch (c) {
        case BallColor::RED:    return 'R';
        case BallColor::GREEN:  return 'G';
        case BallColor::BLUE:   return 'B';
        case BallColor::YELLOW: return 'Y';
        case BallColor::PURPLE: return 'P';
        case BallColor::ORANGE: return 'O';
        default:                return '?';
    }
}

// 将字符转换为颜色
inline BallColor charToColor(char ch) {
    switch (ch) {
        case 'R': return BallColor::RED;
        case 'G': return BallColor::GREEN;
        case 'B': return BallColor::BLUE;
        case 'Y': return BallColor::YELLOW;
        case 'P': return BallColor::PURPLE;
        case 'O': return BallColor::ORANGE;
        default:  return BallColor::RED;
    }
}

// 将颜色转换为字符串（用于JSON）
inline std::string colorToString(BallColor c) {
    switch (c) {
        case BallColor::RED:    return "R";
        case BallColor::GREEN:  return "G";
        case BallColor::BLUE:   return "B";
        case BallColor::YELLOW: return "Y";
        case BallColor::PURPLE: return "P";
        case BallColor::ORANGE: return "O";
        default:                return "?";
    }
}

// 游戏状态快照（用于撤销）
struct GameStateSnapshot {
    std::list<BallColor> track;
    std::queue<BallColor> fireQueue;
    int score;
    bool gameOver;
    bool gameWon;
};

// 道具类型枚举
enum class ItemType {
    SHUFFLE,    // 随机重排轨道上的彩球
    BOMB,       // 消除指定位置周围的所有彩球
    COLOR_BOMB, // 消除轨道上所有指定颜色的彩球
    REVERSE,    // 反转轨道彩球顺序
    COUNT
};

// 道具信息
struct ItemInfo {
    ItemType type;
    int count;  // 可用次数
};

// 游戏结果
struct EliminationResult {
    bool eliminated;           // 是否发生了消除
    int eliminatedCount;       // 消除的彩球数量
    int chainCount;            // 连锁消除次数
    std::vector<int> eliminatedPositions; // 消除的位置
};

// 游戏类
class ZumaGame {
public:
    ZumaGame();
    ~ZumaGame();

    // 初始化游戏
    // trackSize: 轨道初始彩球数量
    // queueSize: 待发射队列彩球数量
    // maxTrack: 轨道最大容量（超过则游戏失败）
    void init(int trackSize = 8, int queueSize = 10, int maxTrack = 15);

    // 在指定位置插入当前待发射彩球
    // pos: 插入位置（0到track.size()）
    // 返回消除结果
    EliminationResult fire(int pos);

    // 撤销上一步操作
    bool undo();

    // 使用道具
    // type: 道具类型
    // pos: 道具作用位置（BOMB需要）
    // color: 道具作用颜色（COLOR_BOMB需要）
    bool useItem(ItemType type, int pos = -1, BallColor color = BallColor::RED);

    // 获取游戏状态（JSON格式）
    std::string getStateJson() const;

    // 获取轨道彩球列表
    const std::list<BallColor>& getTrack() const { return track_; }

    // 获取待发射队列
    const std::queue<BallColor>& getFireQueue() const { return fireQueue_; }

    // 获取当前待发射彩球
    BallColor getCurrentBall() const;

    // 获取分数
    int getScore() const { return score_; }

    // 是否游戏结束
    bool isGameOver() const { return gameOver_; }

    // 是否游戏胜利
    bool isGameWon() const { return gameWon_; }

    // 获取道具列表
    const std::vector<ItemInfo>& getItems() const { return items_; }

    // 获取轨道最大容量
    int getMaxTrack() const { return maxTrack_; }

private:
    // 轨道（链表：支持高效插入和删除）
    std::list<BallColor> track_;

    // 待发射彩球队列（队列：先进先出）
    std::queue<BallColor> fireQueue_;

    // 撤销栈（栈：保存游戏状态快照）
    std::stack<GameStateSnapshot> undoStack_;

    // 道具列表
    std::vector<ItemInfo> items_;

    // 游戏参数
    int maxTrack_;     // 轨道最大容量
    int score_;        // 分数
    bool gameOver_;    // 游戏是否结束
    bool gameWon_;     // 游戏是否胜利

    // 随机数生成器
    std::mt19937 rng_;

    // 生成随机彩球
    BallColor randomBall();

    // 保存当前状态到撤销栈
    void saveState();

    // 执行消除和连锁消除
    EliminationResult eliminate();

    // 检查游戏状态
    void checkGameState();

    // 将list转换为vector（用于JSON）
    std::vector<BallColor> listToVector(const std::list<BallColor>& lst) const;

    // 将queue转换为vector（用于JSON）
    std::vector<BallColor> queueToVector(const std::queue<BallColor>& q) const;
};

// 道具类型转字符串（用于JSON序列化）
inline std::string itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::SHUFFLE:    return "shuffle";
        case ItemType::BOMB:       return "bomb";
        case ItemType::COLOR_BOMB: return "colorBomb";
        case ItemType::REVERSE:    return "reverse";
        default:                   return "unknown";
    }
}

#endif // ZUMA_GAME_HPP