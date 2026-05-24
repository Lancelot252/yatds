#include "zuma_game.hpp"
#include <sstream>
#include <algorithm>

// 可用颜色：R=红, G=绿, B=蓝, Y=黄, P=紫, O=橙
const std::vector<char> ZumaGame::COLORS = {'R', 'G', 'B', 'Y', 'P', 'O'};

ZumaGame::ZumaGame()
    : score_(0), maxTrackSize_(20), comboCount_(0), totalEliminated_(0)
{
    std::random_device rd;
    rng_ = std::mt19937(rd());
}

char ZumaGame::randomColor() {
    std::uniform_int_distribution<int> dist(0, (int)COLORS.size() - 1);
    return COLORS[dist(rng_)];
}

void ZumaGame::init(int trackSize, int queueSize, int maxTrack) {
    track_.clear();
    while (!ballQueue_.empty()) ballQueue_.pop();
    while (!history_.empty()) history_.pop();
    score_ = 0;
    comboCount_ = 0;
    totalEliminated_ = 0;
    maxTrackSize_ = maxTrack;

    // 初始化轨道：生成随机彩球排列
    for (int i = 0; i < trackSize; i++) {
        track_.push_back(randomColor());
    }

    // 初始化待发射队列
    for (int i = 0; i < queueSize; i++) {
        ballQueue_.push(randomColor());
    }

    saveState();
}

std::string ZumaGame::getTrackString() const {
    std::string s;
    for (char c : track_) s += c;
    return s;
}

std::string ZumaGame::getQueueString() const {
    std::string s;
    std::queue<char> q = ballQueue_;
    while (!q.empty()) { s += q.front(); q.pop(); }
    return s;
}

std::vector<char> ZumaGame::getTrack() const {
    return std::vector<char>(track_.begin(), track_.end());
}

std::vector<char> ZumaGame::getQueue() const {
    std::vector<char> v;
    std::queue<char> q = ballQueue_;
    while (!q.empty()) { v.push_back(q.front()); q.pop(); }
    return v;
}

char ZumaGame::getNextBall() const {
    return ballQueue_.empty() ? ' ' : ballQueue_.front();
}

int ZumaGame::getScore() const        { return score_; }
int ZumaGame::getCombo() const        { return comboCount_; }
int ZumaGame::getTotalEliminated() const { return totalEliminated_; }
int ZumaGame::getMaxTrack() const     { return maxTrackSize_; }
int ZumaGame::getTrackSize() const    { return (int)track_.size(); }
int ZumaGame::getQueueSize() const    { return (int)ballQueue_.size(); }
bool ZumaGame::isWin() const          { return track_.empty() && ballQueue_.empty(); }
bool ZumaGame::isLose() const         { return (int)track_.size() >= maxTrackSize_; }
bool ZumaGame::isGameOver() const     { return isWin() || isLose(); }

// ==================== 保存/恢复状态（JSON格式） ====================

void ZumaGame::saveState() {
    history_.push(getStateJSON());
}

std::string ZumaGame::getStateJSON() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"track\":\"" << getTrackString() << "\",";
    oss << "\"queue\":\"" << getQueueString() << "\",";
    oss << "\"score\":" << score_ << ",";
    oss << "\"combo\":" << comboCount_ << ",";
    oss << "\"eliminated\":" << totalEliminated_ << ",";
    oss << "\"maxTrack\":" << maxTrackSize_;
    oss << "}";
    return oss.str();
}

// ==================== 消除算法（核心） ====================

int ZumaGame::checkAndEliminate() {
    int eliminated = 0;

    while (true) {
        if (track_.size() < 3) break;

        bool found = false;
        auto it = track_.begin();
        auto start = it;
        int count = 1;

        // 遍历链表找到连续 >=3 个同色球
        while (it != track_.end()) {
            auto next = std::next(it);
            if (next != track_.end() && *next == *it) {
                count++;
            } else {
                if (count >= 3) {
                    // 消除 count 个球
                    track_.erase(start, next);
                    eliminated += count;
                    found = true;
                    break;  // 消除后重新从头扫描以处理连锁
                }
                start = next;
                count = 1;
            }
            it = next;
        }

        if (!found) break;
    }

    return eliminated;
}

// ==================== 发射彩球 ====================

bool ZumaGame::fire(int position) {
    if (isGameOver()) return false;
    if (ballQueue_.empty()) return false;
    if (position < 0 || position > (int)track_.size()) return false;

    saveState();

    char ball = ballQueue_.front();
    ballQueue_.pop();

    // 插入到链表指定位置
    auto it = track_.begin();
    std::advance(it, position);
    track_.insert(it, ball);

    // 补充待发射队列
    if (ballQueue_.empty()) refillQueue(5);

    // 执行消除
    int eliminated = checkAndEliminate();
    if (eliminated > 0) {
        totalEliminated_ += eliminated;
        comboCount_++;
        score_ += eliminated * 10 * comboCount_;
    } else {
        comboCount_ = 0;
    }

    return true;
}

// ==================== 撤销操作（使用栈） ====================

bool ZumaGame::undo() {
    if (history_.size() <= 1) return false;  // 至少保留初始状态

    // 弹出当前状态（最近一次操作后的状态）
    history_.pop();
    // 获取上一次操作前的状态
    std::string prevState = history_.top();
    history_.pop();  // 也要弹出，因为 saveState 会重新保存

    // 解析 JSON 并恢复状态
    // 简单手动解析
    auto extract = [](const std::string& json, const std::string& key) -> std::string {
        size_t pos = json.find("\"" + key + "\":");
        if (pos == std::string::npos) return "";
        pos += key.size() + 3;  // skip "key":"
        if (json[pos] == '"') pos++;  // 如果有引号
        size_t end = json.find_first_of("\",}", pos);
        if (end == std::string::npos) end = json.size();
        return json.substr(pos, end - pos);
    };

    std::string trackStr = extract(prevState, "track");
    std::string queueStr = extract(prevState, "queue");
    std::string scoreStr = extract(prevState, "score");
    std::string comboStr = extract(prevState, "combo");
    std::string elimStr  = extract(prevState, "eliminated");
    std::string maxStr   = extract(prevState, "maxTrack");

    // 恢复轨道
    track_.clear();
    for (char c : trackStr) track_.push_back(c);

    // 恢复队列
    while (!ballQueue_.empty()) ballQueue_.pop();
    for (char c : queueStr) ballQueue_.push(c);

    score_            = std::stoi(scoreStr);
    comboCount_       = std::stoi(comboStr);
    totalEliminated_  = std::stoi(elimStr);
    maxTrackSize_     = std::stoi(maxStr);

    saveState();  // 保存当前恢复后的状态
    return true;
}

// ==================== 💣 炸弹道具 ====================

bool ZumaGame::bomb(int position) {
    if (isGameOver()) return false;
    if (track_.empty()) return false;
    if (position < 0 || position >= (int)track_.size()) return false;

    saveState();

    // 计算消除范围 [position-2, position+2] 不超过链表边界
    int left = std::max(0, position - 2);
    int right = std::min((int)track_.size() - 1, position + 2);

    auto itStart = track_.begin();
    std::advance(itStart, left);
    auto itEnd = track_.begin();
    std::advance(itEnd, right + 1);

    int eliminated = (int)std::distance(itStart, itEnd);
    track_.erase(itStart, itEnd);

    totalEliminated_ += eliminated;
    comboCount_++;
    score_ += eliminated * 5;

    // 炸弹后检查连锁消除
    int chain = checkAndEliminate();
    if (chain > 0) {
        totalEliminated_ += chain;
        score_ += chain * 10;
    }

    return true;
}

// ==================== 🔀 随机变换道具 ====================

void ZumaGame::shuffleNextBall() {
    if (ballQueue_.empty()) return;

    saveState();

    // 取出并改变颜色
    char ball = ballQueue_.front();
    ballQueue_.pop();

    char newColor;
    do {
        newColor = randomColor();
    } while (newColor == ball);

    // 放回一个新颜色的球到队列头部
    std::queue<char> temp;
    temp.push(newColor);
    while (!ballQueue_.empty()) {
        temp.push(ballQueue_.front());
        ballQueue_.pop();
    }
    ballQueue_ = std::move(temp);
}

// ==================== ⏪ 轨道回退道具 ====================

bool ZumaGame::pushBack() {
    if (isGameOver()) return false;
    if (track_.empty()) return false;
    if ((int)track_.size() >= maxTrackSize_) return false;

    saveState();

    // 移除尾部彩球
    track_.pop_back();
    // 在头部插入新随机彩球
    track_.push_front(randomColor());

    // 检查是否有新的消除
    int eliminated = checkAndEliminate();
    if (eliminated > 0) {
        totalEliminated_ += eliminated;
        score_ += eliminated * 10;
    }

    return true;
}

// ==================== 🌈 彩虹球道具 ====================

bool ZumaGame::rainbow(int position) {
    if (isGameOver()) return false;
    if (track_.empty()) return false;
    if (position < 0 || position >= (int)track_.size()) return false;

    saveState();

    // 找到相邻颜色
    char targetColor = 0;
    auto it = track_.begin();
    std::advance(it, position);

    if (position > 0) {
        auto prev = std::prev(it);
        targetColor = *prev;
    }
    if (targetColor == 0 && position < (int)track_.size() - 1) {
        auto next = std::next(it);
        targetColor = *next;
    }
    if (targetColor == 0) return false;  // 无法确定目标颜色

    *it = targetColor;

    // 检查消除
    int eliminated = checkAndEliminate();
    if (eliminated > 0) {
        totalEliminated_ += eliminated;
        comboCount_++;
        score_ += eliminated * 15;
    } else {
        comboCount_ = 0;
    }

    return true;
}

// ==================== 补充队列 ====================

void ZumaGame::refillQueue(int count) {
    for (int i = 0; i < count; i++) {
        ballQueue_.push(randomColor());
    }
}