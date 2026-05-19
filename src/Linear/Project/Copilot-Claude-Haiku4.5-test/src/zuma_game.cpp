#include "zuma_game.hpp"
#include <iostream>

ZumaGame::ZumaGame() : eliminations(0) {}

void ZumaGame::init(const std::string& trackStr, const std::string& queueStr) {
    track.clear();
    eliminations = 0;
    
    // 初始化轨道
    for (char c : trackStr) {
        if (c != ' ') {
            track.push_back(Ball(Ball::fromChar(c)));
        }
    }
    
    // 初始化发射队列
    while (!shootQueue.empty()) {
        shootQueue.pop();
    }
    
    for (char c : queueStr) {
        if (c != ' ') {
            shootQueue.push(Ball(Ball::fromChar(c)));
        }
    }
    
    // 保存初始状态
    saveState();
}

void ZumaGame::saveState() {
    GameState state;
    state.track = track;
    state.shootQueue = shootQueue;
    history.push_back(state);
}

bool ZumaGame::insertBall(int position, const Ball& ball) {
    // 检查position是否有效
    if (position < 0 || position > (int)track.size()) {
        return false;
    }
    
    // 保存当前状态
    saveState();
    
    // 插入彩球
    track.insert(track.begin() + position, ball);
    
    // 执行消除
    eliminate();
    
    return true;
}

Ball ZumaGame::getNextBall() {
    if (shootQueue.empty()) {
        return Ball(BallColor::NONE);
    }
    
    Ball ball = shootQueue.front();
    shootQueue.pop();
    return ball;
}

bool ZumaGame::isEmpty() const {
    return track.empty();
}

std::string ZumaGame::getTrackStatus() const {
    std::string result = "";
    for (const auto& ball : track) {
        result += ball.toString();
        result += " ";
    }
    return result.empty() ? "(empty)" : result;
}

std::string ZumaGame::getQueueStatus() const {
    std::queue<Ball> temp = shootQueue;
    std::string result = "";
    while (!temp.empty()) {
        result += temp.front().toString();
        result += " ";
        temp.pop();
    }
    return result.empty() ? "(empty)" : result;
}

bool ZumaGame::isWin() const {
    return track.empty() && shootQueue.empty();
}

bool ZumaGame::isLose() const {
    // 如果轨道满了（例如超过50个球）则游戏失败
    return track.size() > 50;
}

bool ZumaGame::undo() {
    if (history.size() <= 1) {
        return false; // 无法撤销初始状态
    }
    
    history.pop_back(); // 移除当前状态
    GameState& prevState = history.back();
    
    track = prevState.track;
    shootQueue = prevState.shootQueue;
    
    return true;
}

void ZumaGame::eliminate() {
    bool hasElimination = true;
    
    while (hasElimination) {
        hasElimination = eliminateSequence();
    }
}

bool ZumaGame::eliminateSequence() {
    std::vector<bool> toRemove(track.size(), false);
    bool found = false;
    
    for (size_t i = 0; i < track.size(); ) {
        size_t j = i;
        // 找出连续相同颜色的球
        while (j < track.size() && track[j].color == track[i].color) {
            j++;
        }
        
        // 如果连续3个或以上，标记为删除
        if (j - i >= 3) {
            for (size_t k = i; k < j; k++) {
                toRemove[k] = true;
            }
            found = true;
            eliminations++;
        }
        
        i = j;
    }
    
    // 删除标记的球
    std::vector<Ball> newTrack;
    for (size_t i = 0; i < track.size(); i++) {
        if (!toRemove[i]) {
            newTrack.push_back(track[i]);
        }
    }
    
    track = newTrack;
    return found;
}

bool ZumaGame::checkAndEliminate(size_t pos) {
    if (pos >= track.size()) {
        return false;
    }
    
    BallColor color = track[pos].color;
    
    // 向左扩展
    int left = pos;
    while (left > 0 && track[left - 1].color == color) {
        left--;
    }
    
    // 向右扩展
    size_t right = pos;
    while (right < track.size() - 1 && track[right + 1].color == color) {
        right++;
    }
    
    // 如果连续3个或以上
    if (right - left + 1 >= 3) {
        track.erase(track.begin() + left, track.begin() + right + 1);
        eliminations++;
        return true;
    }
    
    return false;
}

size_t ZumaGame::getTrackSize() const {
    return track.size();
}

size_t ZumaGame::getQueueSize() const {
    return shootQueue.size();
}

void ZumaGame::reset() {
    track.clear();
    while (!shootQueue.empty()) {
        shootQueue.pop();
    }
    history.clear();
    eliminations = 0;
}

int ZumaGame::getEliminations() const {
    return eliminations;
}
