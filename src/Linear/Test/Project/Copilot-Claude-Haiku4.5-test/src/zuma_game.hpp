#ifndef ZUMA_GAME_HPP
#define ZUMA_GAME_HPP

#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>

// 彩球颜色定义
enum class BallColor {
    RED = 0,
    BLUE = 1,
    GREEN = 2,
    YELLOW = 3,
    PURPLE = 4,
    NONE = 5
};

// 单个彩球
struct Ball {
    BallColor color;
    
    Ball() : color(BallColor::NONE) {}
    Ball(BallColor c) : color(c) {}
    
    std::string toString() const {
        switch(color) {
            case BallColor::RED: return "R";
            case BallColor::BLUE: return "B";
            case BallColor::GREEN: return "G";
            case BallColor::YELLOW: return "Y";
            case BallColor::PURPLE: return "P";
            default: return "N";
        }
    }
    
    static BallColor fromChar(char c) {
        switch(c) {
            case 'R': return BallColor::RED;
            case 'B': return BallColor::BLUE;
            case 'G': return BallColor::GREEN;
            case 'Y': return BallColor::YELLOW;
            case 'P': return BallColor::PURPLE;
            default: return BallColor::NONE;
        }
    }
};

// 游戏状态快照，用于撤销功能
struct GameState {
    std::vector<Ball> track;
    std::queue<Ball> shootQueue;
};

// 祖玛游戏类
class ZumaGame {
private:
    std::vector<Ball> track;           // 轨道上的彩球
    std::queue<Ball> shootQueue;       // 待发射的彩球队列
    std::vector<GameState> history;    // 历史状态，用于撤销
    int eliminations;                  // 消除次数统计
    
public:
    ZumaGame();
    
    // 初始化游戏
    void init(const std::string& trackStr, const std::string& queueStr);
    
    // 在指定位置插入彩球
    bool insertBall(int position, const Ball& ball);
    
    // 从发射队列中取出下一个球
    Ball getNextBall();
    
    // 检查轨道中是否还有球
    bool isEmpty() const;
    
    // 获取轨道信息
    std::string getTrackStatus() const;
    
    // 获取发射队列状态
    std::string getQueueStatus() const;
    
    // 游戏胜负判断
    bool isWin() const;
    bool isLose() const;
    
    // 撤销操作
    bool undo();
    
    // 保存当前状态
    void saveState();
    
    // 获取轨道大小
    size_t getTrackSize() const;
    
    // 获取队列大小
    size_t getQueueSize() const;
    
    // 重置游戏
    void reset();
    
    // 获取消除数
    int getEliminations() const;
    
private:
    // 执行消除操作（包括连锁消除）
    void eliminate();
    
    // 查找并消除连续三个或以上相同颜色的球
    bool eliminateSequence();
    
    // 辅助函数：检查位置i周围是否有连续三个或以上相同颜色的球
    bool checkAndEliminate(size_t pos);
};

#endif // ZUMA_GAME_HPP
