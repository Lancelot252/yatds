#include "zuma_game.hpp"
#include <sstream>
#include <chrono>

ZumaGame::ZumaGame()
    : maxTrack_(15)
    , score_(0)
    , gameOver_(false)
    , gameWon_(false)
    , rng_(std::chrono::system_clock::now().time_since_epoch().count()) {
}

ZumaGame::~ZumaGame() = default;

BallColor ZumaGame::randomBall() {
    // 随机生成0到COUNT-1之间的颜色
    std::uniform_int_distribution<int> dist(0, static_cast<int>(BallColor::COUNT) - 1);
    return static_cast<BallColor>(dist(rng_));
}

void ZumaGame::init(int trackSize, int queueSize, int maxTrack) {
    maxTrack_ = maxTrack;
    score_ = 0;
    gameOver_ = false;
    gameWon_ = false;

    // 清空所有数据结构
    track_.clear();
    while (!fireQueue_.empty()) fireQueue_.pop();
    while (!undoStack_.empty()) undoStack_.pop();
    items_.clear();

    // 初始化轨道：随机生成彩球
    for (int i = 0; i < trackSize; ++i) {
        track_.push_back(randomBall());
    }

    // 初始化待发射队列：随机生成彩球
    for (int i = 0; i < queueSize; ++i) {
        fireQueue_.push(randomBall());
    }

    // 初始化道具：每种道具给2次
    for (int i = 0; i < static_cast<int>(ItemType::COUNT); ++i) {
        items_.push_back({static_cast<ItemType>(i), 2});
    }
}

BallColor ZumaGame::getCurrentBall() const {
    if (fireQueue_.empty()) {
        return BallColor::RED; // 默认返回
    }
    return fireQueue_.front();
}

void ZumaGame::saveState() {
    GameStateSnapshot snapshot;
    snapshot.track = track_;
    snapshot.fireQueue = fireQueue_;
    snapshot.score = score_;
    snapshot.gameOver = gameOver_;
    snapshot.gameWon = gameWon_;
    undoStack_.push(snapshot);
}

EliminationResult ZumaGame::eliminate() {
    EliminationResult result;
    result.eliminated = false;
    result.eliminatedCount = 0;
    result.chainCount = 0;

    bool foundElimination = true;

    while (foundElimination) {
        foundElimination = false;

        if (track_.size() < 3) break;

        // 遍历链表寻找连续3个及以上相同颜色的彩球
        auto it = track_.begin();
        while (it != track_.end()) {
            auto start = it;
            BallColor currentColor = *it;
            int count = 0;

            // 计算从当前位置开始连续相同颜色的数量
            while (it != track_.end() && *it == currentColor) {
                ++count;
                ++it;
            }

            if (count >= 3) {
                // 找到连续3个及以上相同颜色，执行消除
                foundElimination = true;
                result.eliminated = true;
                result.chainCount++;
                result.eliminatedCount += count;

                // 记录消除位置（简化处理，不记录具体位置）
                // 消除从start到it之前的所有彩球
                auto eraseEnd = it;
                track_.erase(start, eraseEnd);

                // 消除后加分：连锁消除越多分数越高
                score_ += count * 10 * result.chainCount;

                // 消除后需要重新从头检查（因为可能产生新的连锁）
                it = track_.begin();
                break;
            }
        }
    }

    return result;
}

EliminationResult ZumaGame::fire(int pos) {
    if (gameOver_ || gameWon_) {
        EliminationResult r;
        r.eliminated = false;
        return r;
    }

    if (fireQueue_.empty()) {
        EliminationResult r;
        r.eliminated = false;
        return r;
    }

    // 保存当前状态（用于撤销）
    saveState();

    // 从队列中取出一个彩球
    BallColor ball = fireQueue_.front();
    fireQueue_.pop();

    // 在指定位置插入彩球
    // pos范围：0到track_.size()
    if (pos < 0) pos = 0;
    if (pos > static_cast<int>(track_.size())) pos = static_cast<int>(track_.size());

    auto it = track_.begin();
    std::advance(it, pos);
    track_.insert(it, ball);

    // 执行消除和连锁消除
    EliminationResult result = eliminate();

    // 检查游戏状态
    checkGameState();

    return result;
}

bool ZumaGame::undo() {
    if (undoStack_.empty()) {
        return false;
    }

    GameStateSnapshot snapshot = undoStack_.top();
    undoStack_.pop();

    track_ = snapshot.track;
    fireQueue_ = snapshot.fireQueue;
    score_ = snapshot.score;
    gameOver_ = snapshot.gameOver;
    gameWon_ = snapshot.gameWon;

    return true;
}

bool ZumaGame::useItem(ItemType type, int pos, BallColor color) {
    if (gameOver_ || gameWon_) return false;

    // 查找道具
    int itemIndex = -1;
    for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
        if (items_[i].type == type && items_[i].count > 0) {
            itemIndex = i;
            break;
        }
    }

    if (itemIndex == -1) return false;

    // 保存状态用于撤销
    saveState();

    // 使用道具
    items_[itemIndex].count--;

    switch (type) {
        case ItemType::SHUFFLE: {
            // 随机重排轨道上的彩球
            std::vector<BallColor> temp(track_.begin(), track_.end());
            std::shuffle(temp.begin(), temp.end(), rng_);
            track_.clear();
            for (const auto& b : temp) {
                track_.push_back(b);
            }
            // 重排后检查是否有可消除的
            eliminate();
            break;
        }
        case ItemType::BOMB: {
            // 消除指定位置前后各2个彩球（共最多5个）
            if (pos < 0 || pos >= static_cast<int>(track_.size())) {
                items_[itemIndex].count++; // 退还道具
                return false;
            }
            auto it = track_.begin();
            std::advance(it, pos);

            // 计算消除范围
            int startOffset = std::max(0, pos - 2);
            int endOffset = std::min(static_cast<int>(track_.size()) - 1, pos + 2);

            auto startIt = track_.begin();
            std::advance(startIt, startOffset);
            auto endIt = track_.begin();
            std::advance(endIt, endOffset + 1);

            int removed = endOffset - startOffset + 1;
            track_.erase(startIt, endIt);
            score_ += removed * 15;

            // 消除后检查连锁
            eliminate();
            break;
        }
        case ItemType::COLOR_BOMB: {
            // 消除轨道上所有指定颜色的彩球
            int removed = 0;
            auto it = track_.begin();
            while (it != track_.end()) {
                if (*it == color) {
                    it = track_.erase(it);
                    removed++;
                } else {
                    ++it;
                }
            }
            score_ += removed * 20;

            // 消除后检查连锁
            eliminate();
            break;
        }
        case ItemType::REVERSE: {
            // 反转轨道彩球顺序
            std::list<BallColor> reversed;
            for (auto it = track_.rbegin(); it != track_.rend(); ++it) {
                reversed.push_back(*it);
            }
            track_ = reversed;

            // 反转后检查是否有可消除的
            eliminate();
            break;
        }
        default:
            items_[itemIndex].count++; // 退还道具
            return false;
    }

    checkGameState();
    return true;
}

void ZumaGame::checkGameState() {
    // 轨道为空，游戏胜利
    if (track_.empty()) {
        gameWon_ = true;
        score_ += 100; // 胜利奖励
        return;
    }

    // 轨道超过最大容量，游戏失败
    if (static_cast<int>(track_.size()) > maxTrack_) {
        gameOver_ = true;
        return;
    }

    // 待发射队列空且轨道不为空，且轨道没有可消除的
    // 这里不主动判断失败，让玩家继续尝试
}

std::vector<BallColor> ZumaGame::listToVector(const std::list<BallColor>& lst) const {
    return std::vector<BallColor>(lst.begin(), lst.end());
}

std::vector<BallColor> ZumaGame::queueToVector(const std::queue<BallColor>& q) const {
    std::queue<BallColor> temp = q;
    std::vector<BallColor> result;
    while (!temp.empty()) {
        result.push_back(temp.front());
        temp.pop();
    }
    return result;
}

std::string ZumaGame::getStateJson() const {
    std::ostringstream oss;
    oss << "{";

    // 轨道
    oss << "\"track\": [";
    auto trackVec = listToVector(track_);
    for (size_t i = 0; i < trackVec.size(); ++i) {
        oss << "\"" << colorToString(trackVec[i]) << "\"";
        if (i + 1 < trackVec.size()) oss << ",";
    }
    oss << "],";

    // 待发射队列
    oss << "\"queue\": [";
    auto queueVec = queueToVector(fireQueue_);
    for (size_t i = 0; i < queueVec.size(); ++i) {
        oss << "\"" << colorToString(queueVec[i]) << "\"";
        if (i + 1 < queueVec.size()) oss << ",";
    }
    oss << "],";

    // 当前彩球
    oss << "\"currentBall\": \"" << colorToString(getCurrentBall()) << "\",";

    // 分数
    oss << "\"score\": " << score_ << ",";

    // 游戏状态
    oss << "\"gameOver\": " << (gameOver_ ? "true" : "false") << ",";
    oss << "\"gameWon\": " << (gameWon_ ? "true" : "false") << ",";

    // 轨道最大容量
    oss << "\"maxTrack\": " << maxTrack_ << ",";

    // 道具
    oss << "\"items\": [";
    for (size_t i = 0; i < items_.size(); ++i) {
        oss << "{";
        oss << "\"type\": " << static_cast<int>(items_[i].type) << ",";
        oss << "\"name\": \"" << itemTypeToString(items_[i].type) << "\",";
        oss << "\"count\": " << items_[i].count;
        oss << "}";
        if (i + 1 < items_.size()) oss << ",";
    }
    oss << "],";

    // 可撤销次数
    oss << "\"undoCount\": " << undoStack_.size();

    oss << "}";
    return oss.str();
}

