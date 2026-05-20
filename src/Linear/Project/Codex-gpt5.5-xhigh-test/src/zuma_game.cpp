#include "zuma_game.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>

namespace {

std::string quoteBall(char ball) {
    std::string result = "\"";
    result.push_back(ball);
    result.push_back('"');
    return result;
}

} // namespace

ZumaGame::ZumaGame() {
    reset();
}

void ZumaGame::reset() {
    while (!undoStack_.empty()) {
        undoStack_.pop();
    }

    track_.clear();
    launcher_ = std::queue<char>();

    const std::vector<char> initialTrack = {
        'R', 'G', 'B', 'B', 'Y', 'P', 'G', 'R',
        'C', 'Y', 'P', 'P', 'C', 'B', 'G', 'R'
    };
    const std::vector<char> initialLauncher = {
        'B', 'Y', 'P', 'R', 'R', 'G', 'C', 'C',
        'B', 'B', 'Y', 'P', 'G', 'R', 'C', 'Y',
        'P', 'G', 'R', 'B', 'C', 'Y', 'P', 'G',
        'R', 'B', 'C', 'Y', 'P', 'G', 'R', 'B',
        'C', 'Y', 'P', 'G', 'R', 'B', 'C', 'Y'
    };

    for (char ball : initialTrack) {
        track_.push_back(ball);
    }
    for (char ball : initialLauncher) {
        launcher_.push(ball);
    }

    score_ = 0;
    moves_ = 0;
    bombs_ = 2;
    sweeps_ = 1;
    rotates_ = 4;
    lastCombo_ = 0;
    status_ = Status::Playing;
    message_ = "新游戏开始。选择插入槽位并发射当前彩球。";
}

bool ZumaGame::insertBall(std::size_t position) {
    if (status_ != Status::Playing) {
        message_ = "游戏已经结束，请重新开始。";
        return false;
    }
    if (launcher_.empty()) {
        message_ = "待发射队列为空，无法继续发射。";
        checkGameOver();
        return false;
    }
    if (position > track_.size()) {
        message_ = "插入位置超出轨道范围。";
        return false;
    }

    pushUndo();

    const char ball = launcher_.front();
    launcher_.pop();
    track_.insert(iteratorAt(position), ball);
    ++moves_;

    const int removed = resolveEliminations();
    checkGameOver();

    std::ostringstream out;
    out << "发射 " << ball << " 到槽位 " << position << "。";
    if (removed > 0) {
        out << " 消除了 " << removed << " 个彩球";
        if (lastCombo_ > 1) {
            out << "，形成 " << lastCombo_ << " 段连锁";
        }
        out << "。";
    } else {
        out << " 未形成消除。";
    }
    if (status_ == Status::Won) {
        out << " 轨道清空，胜利。";
    } else if (status_ == Status::Lost) {
        out << " 游戏失败。";
    }
    message_ = out.str();
    return true;
}

bool ZumaGame::undo() {
    if (undoStack_.empty()) {
        message_ = "没有可以撤销的操作。";
        return false;
    }

    const Snapshot snapshot = undoStack_.top();
    undoStack_.pop();
    restoreSnapshot(snapshot);
    lastCombo_ = 0;
    message_ = "已撤销上一步操作。";
    return true;
}

bool ZumaGame::usePowerup(const std::string& type, std::size_t position) {
    if (status_ != Status::Playing) {
        message_ = "游戏已经结束，请重新开始。";
        return false;
    }
    if (track_.empty()) {
        message_ = "轨道为空，不需要使用道具。";
        checkGameOver();
        return false;
    }
    if (position >= track_.size()) {
        message_ = "道具目标位置超出轨道范围。";
        return false;
    }

    if (type == "bomb") {
        if (bombs_ <= 0) {
            message_ = "爆破道具已经用完。";
            return false;
        }

        pushUndo();

        auto center = iteratorAt(position);
        auto begin = center;
        if (begin != track_.begin()) {
            --begin;
        }
        auto end = center;
        ++end;
        if (end != track_.end()) {
            ++end;
        }

        const int removedByBomb = static_cast<int>(std::distance(begin, end));
        track_.erase(begin, end);
        --bombs_;
        ++moves_;
        score_ += removedByBomb * 8;

        const int chained = resolveEliminations();
        checkGameOver();

        std::ostringstream out;
        out << "爆破道具移除了 " << removedByBomb << " 个彩球。";
        if (chained > 0) {
            out << " 之后连锁消除了 " << chained << " 个彩球。";
        }
        if (status_ == Status::Won) {
            out << " 轨道清空，胜利。";
        }
        message_ = out.str();
        return true;
    }

    if (type == "sweep") {
        if (sweeps_ <= 0) {
            message_ = "同色清除道具已经用完。";
            return false;
        }

        pushUndo();

        const char target = *constIteratorAt(position);
        int removed = 0;
        for (auto it = track_.begin(); it != track_.end();) {
            if (*it == target) {
                it = track_.erase(it);
                ++removed;
            } else {
                ++it;
            }
        }

        --sweeps_;
        ++moves_;
        score_ += removed * 6;

        const int chained = resolveEliminations();
        checkGameOver();

        std::ostringstream out;
        out << "同色清除移除了所有 " << target << " 彩球，共 " << removed << " 个。";
        if (chained > 0) {
            out << " 之后连锁消除了 " << chained << " 个彩球。";
        }
        if (status_ == Status::Won) {
            out << " 轨道清空，胜利。";
        }
        message_ = out.str();
        return true;
    }

    message_ = "未知道具类型。";
    return false;
}

bool ZumaGame::rotateLauncher() {
    if (status_ != Status::Playing) {
        message_ = "游戏已经结束，请重新开始。";
        return false;
    }
    if (rotates_ <= 0) {
        message_ = "换球次数已经用完。";
        return false;
    }
    if (launcher_.size() < 2) {
        message_ = "待发射队列不足两个彩球，无法换球。";
        return false;
    }

    pushUndo();

    const char ball = launcher_.front();
    launcher_.pop();
    launcher_.push(ball);
    --rotates_;
    ++moves_;

    message_ = "已将当前彩球移到待发射队列末尾。";
    return true;
}

std::string ZumaGame::stateJson() const {
    std::ostringstream out;
    const std::vector<char> launcher = launcherVector();

    out << "{";
    out << "\"track\":[";
    for (auto it = track_.begin(); it != track_.end(); ++it) {
        if (it != track_.begin()) {
            out << ",";
        }
        out << quoteBall(*it);
    }
    out << "],";

    out << "\"launcher\":[";
    const std::size_t previewCount = std::min<std::size_t>(launcher.size(), 10);
    for (std::size_t i = 0; i < previewCount; ++i) {
        if (i > 0) {
            out << ",";
        }
        out << quoteBall(launcher[i]);
    }
    out << "],";

    out << "\"currentBall\":";
    if (launcher.empty()) {
        out << "null";
    } else {
        out << quoteBall(launcher.front());
    }
    out << ",";

    out << "\"trackSize\":" << track_.size() << ",";
    out << "\"maxTrackSize\":" << kMaxTrackSize << ",";
    out << "\"launcherSize\":" << launcher.size() << ",";
    out << "\"score\":" << score_ << ",";
    out << "\"moves\":" << moves_ << ",";
    out << "\"combo\":" << lastCombo_ << ",";
    out << "\"status\":\"" << statusText() << "\",";
    out << "\"canUndo\":" << (undoStack_.empty() ? "false" : "true") << ",";
    out << "\"powerups\":{";
    out << "\"bomb\":" << bombs_ << ",";
    out << "\"sweep\":" << sweeps_ << ",";
    out << "\"rotate\":" << rotates_;
    out << "},";
    out << "\"message\":\"";
    for (char ch : message_) {
        if (ch == '"' || ch == '\\') {
            out << '\\';
        }
        if (ch == '\n') {
            out << "\\n";
        } else {
            out << ch;
        }
    }
    out << "\"";
    out << "}";
    return out.str();
}

std::list<char>::iterator ZumaGame::iteratorAt(std::size_t position) {
    auto it = track_.begin();
    std::advance(it, static_cast<long>(position));
    return it;
}

std::list<char>::const_iterator ZumaGame::constIteratorAt(std::size_t position) const {
    auto it = track_.cbegin();
    std::advance(it, static_cast<long>(position));
    return it;
}

ZumaGame::Snapshot ZumaGame::makeSnapshot() const {
    Snapshot snapshot;
    snapshot.track.assign(track_.begin(), track_.end());
    snapshot.launcher = launcherVector();
    snapshot.score = score_;
    snapshot.moves = moves_;
    snapshot.bombs = bombs_;
    snapshot.sweeps = sweeps_;
    snapshot.rotates = rotates_;
    snapshot.status = status_;
    return snapshot;
}

void ZumaGame::restoreSnapshot(const Snapshot& snapshot) {
    track_.assign(snapshot.track.begin(), snapshot.track.end());
    launcher_ = std::queue<char>();
    for (char ball : snapshot.launcher) {
        launcher_.push(ball);
    }
    score_ = snapshot.score;
    moves_ = snapshot.moves;
    bombs_ = snapshot.bombs;
    sweeps_ = snapshot.sweeps;
    rotates_ = snapshot.rotates;
    status_ = snapshot.status;
}

void ZumaGame::pushUndo() {
    undoStack_.push(makeSnapshot());
}

int ZumaGame::resolveEliminations() {
    int totalRemoved = 0;
    int chains = 0;
    bool removedInPass = true;

    while (removedInPass) {
        removedInPass = false;
        for (auto it = track_.begin(); it != track_.end();) {
            auto runBegin = it;
            const char color = *it;
            int count = 0;
            while (it != track_.end() && *it == color) {
                ++it;
                ++count;
            }

            if (count >= 3) {
                track_.erase(runBegin, it);
                totalRemoved += count;
                ++chains;
                removedInPass = true;
                break;
            }
        }
    }

    lastCombo_ = chains;
    if (totalRemoved > 0) {
        score_ += totalRemoved * 10;
        if (chains > 1) {
            score_ += (chains - 1) * 25;
        }
    }
    return totalRemoved;
}

void ZumaGame::checkGameOver() {
    if (track_.empty()) {
        status_ = Status::Won;
        return;
    }
    if (track_.size() >= kMaxTrackSize || launcher_.empty()) {
        status_ = Status::Lost;
        return;
    }
    status_ = Status::Playing;
}

std::vector<char> ZumaGame::launcherVector() const {
    std::queue<char> copy = launcher_;
    std::vector<char> result;
    while (!copy.empty()) {
        result.push_back(copy.front());
        copy.pop();
    }
    return result;
}

std::string ZumaGame::statusText() const {
    switch (status_) {
    case Status::Playing:
        return "playing";
    case Status::Won:
        return "won";
    case Status::Lost:
        return "lost";
    }
    return "playing";
}
