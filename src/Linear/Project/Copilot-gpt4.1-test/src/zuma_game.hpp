#pragma once
#include <vector>
#include <string>
#include <stack>

class ZumaGame {
public:
    ZumaGame(const std::vector<char>& initTrack, const std::vector<char>& initQueue);
    // 插入彩球，返回消除信息
    std::string fire(int pos);
    // 撤销上一步
    bool undo();
    // 获取当前轨道
    std::vector<char> getTrack() const;
    // 获取待发射队列
    std::vector<char> getQueue() const;
    // 判断是否胜利
    bool isWin() const;
    // 判断是否失败
    bool isLose() const;
private:
    std::vector<char> track;
    std::vector<char> queue;
    std::stack<std::pair<std::vector<char>, std::vector<char>>> history;
    void eliminate();
};
