#pragma once
#include "LinkedList.hpp"
#include "Queue.hpp"
#include "Stack.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <random>
#include <sstream>

// 彩球颜色编号。0 表示空（不会出现在轨道上）。
// 1..NUM_COLORS 是有效颜色。
struct Ball {
    int color;
    Ball() : color(0) {}
    Ball(int c) : color(c) {}
};

enum class GameStatus { Playing, Won, Lost };

// 描述一次操作的结果，便于前端显示。
struct ActionResult {
    bool success = false;
    std::string message;
    int eliminatedTotal = 0;     // 本次操作累计消除的球数
    int chainCount = 0;          // 连锁消除发生的轮数（>=2 即触发了连锁）
};

class Game {
public:
    // 配置
    int numColors = 5;            // 颜色数量
    int queueLength = 20;         // 待发射球的总数（决定游戏长度）
    int initialTrackLength = 10;  // 初始轨道彩球数量
    size_t maxTrackLength = 30;   // 轨道长度上限，超过则判负
    int bombUses = 2;             // 炸弹道具次数
    int recolorUses = 2;          // 变色道具次数
    int skipUses = 3;             // 跳过道具次数
    int score = 0;

    Game() { reset(0); }

    // 用给定随机种子（0 表示按时间）重置整局游戏。
    void reset(unsigned seed) {
        if (seed == 0) seed = static_cast<unsigned>(std::random_device{}());
        rng.seed(seed);
        track.clear();
        pending.clear();
        history.clear();
        score = 0;
        status = GameStatus::Playing;
        bombUses = 2;
        recolorUses = 2;
        skipUses = 3;

        // 初始化轨道，避免开局已经存在三连色。
        for (int i = 0; i < initialTrackLength; ++i) {
            int c;
            do {
                c = randomColor();
            } while (wouldCreateTriple(static_cast<size_t>(i), c));
            track.pushBack(Ball(c));
        }
        // 初始化待发射队列。
        for (int i = 0; i < queueLength; ++i) {
            pending.enqueue(Ball(randomColor()));
        }
        updateStatus();
    }

    // 在 idx 位置插入当前队首球。idx 范围 [0, track.size()]。
    ActionResult shoot(size_t idx) {
        ActionResult r;
        if (status != GameStatus::Playing) {
            r.message = "游戏已结束";
            return r;
        }
        Ball b;
        if (!pending.peek(b)) {
            r.message = "已没有可发射的彩球";
            return r;
        }
        if (idx > track.size()) idx = track.size();

        snapshot();  // 保存撤销点
        pending.dequeue(b);
        track.insertAt(idx, b);

        int total = 0, chain = 0;
        eliminateAround(idx, total, chain);

        r.success = true;
        r.eliminatedTotal = total;
        r.chainCount = chain;
        score += total * (1 + (chain > 1 ? chain : 0));
        if (total > 0) {
            std::ostringstream os;
            os << "消除 " << total << " 球";
            if (chain >= 2) os << "，触发 " << chain << " 连锁";
            r.message = os.str();
        } else {
            r.message = "已插入";
        }
        updateStatus();
        return r;
    }

    // 道具：炸弹 - 以指定位置为中心移除最多 3 个球（不触发连锁判断之外的额外效果）。
    ActionResult useBomb(size_t idx) {
        ActionResult r;
        if (status != GameStatus::Playing) { r.message = "游戏已结束"; return r; }
        if (bombUses <= 0) { r.message = "炸弹已耗尽"; return r; }
        if (track.empty()) { r.message = "轨道为空"; return r; }
        if (idx >= track.size()) idx = track.size() - 1;

        snapshot();
        --bombUses;

        size_t start = idx > 0 ? idx - 1 : 0;
        size_t end = idx + 1;
        if (end >= track.size()) end = track.size() - 1;

        int removed = 0;
        for (size_t i = start; i <= end && !track.empty();) {
            auto* nd = track.nodeAt(start);
            if (!nd) break;
            track.remove(nd);
            ++removed;
            if (start > end || track.empty()) break;
            --end;
            if (end < start) break;
        }

        int total = removed, chain = 0;
        // 炸弹后，从 start 位置继续检查可能的连锁
        if (!track.empty()) {
            size_t check = start < track.size() ? start : track.size() - 1;
            int extra = 0;
            eliminateAround(check, extra, chain);
            total += extra;
        }

        r.success = true;
        r.eliminatedTotal = total;
        r.chainCount = chain;
        score += total * 2;
        std::ostringstream os;
        os << "炸弹消除 " << total << " 球";
        r.message = os.str();
        updateStatus();
        return r;
    }

    // 道具：变色 - 把指定位置的球颜色改为目标颜色，并触发可能的消除。
    ActionResult useRecolor(size_t idx, int color) {
        ActionResult r;
        if (status != GameStatus::Playing) { r.message = "游戏已结束"; return r; }
        if (recolorUses <= 0) { r.message = "变色道具已耗尽"; return r; }
        if (color < 1 || color > numColors) { r.message = "无效颜色"; return r; }
        auto* nd = track.nodeAt(idx);
        if (!nd) { r.message = "无效位置"; return r; }

        snapshot();
        --recolorUses;
        nd->value.color = color;

        int total = 0, chain = 0;
        eliminateAround(idx, total, chain);
        r.success = true;
        r.eliminatedTotal = total;
        r.chainCount = chain;
        score += total;
        std::ostringstream os;
        os << "变色，消除 " << total << " 球";
        r.message = os.str();
        updateStatus();
        return r;
    }

    // 道具：跳过 - 把当前队首球放到队尾。
    ActionResult useSkip() {
        ActionResult r;
        if (status != GameStatus::Playing) { r.message = "游戏已结束"; return r; }
        if (skipUses <= 0) { r.message = "跳过道具已耗尽"; return r; }
        if (pending.size() < 2) { r.message = "队列中球数不足"; return r; }
        snapshot();
        --skipUses;
        pending.sendToBack();
        r.success = true;
        r.message = "已跳过当前球";
        return r;
    }

    // 撤销：从快照栈弹出最近一次状态。
    ActionResult undo() {
        ActionResult r;
        Snapshot s;
        if (!history.pop(s)) {
            r.message = "没有可撤销的操作";
            return r;
        }
        applySnapshot(s);
        updateStatus();
        r.success = true;
        r.message = "已撤销";
        return r;
    }

    // 把当前状态序列化成简单的 JSON 字符串。
    std::string toJson() const {
        std::ostringstream os;
        os << "{";
        os << "\"status\":\"" << statusString() << "\",";
        os << "\"score\":" << score << ",";
        os << "\"numColors\":" << numColors << ",";
        os << "\"maxTrackLength\":" << maxTrackLength << ",";
        os << "\"bombUses\":" << bombUses << ",";
        os << "\"recolorUses\":" << recolorUses << ",";
        os << "\"skipUses\":" << skipUses << ",";
        os << "\"undoAvailable\":" << (history.size() > 0 ? "true" : "false") << ",";

        os << "\"track\":[";
        auto tv = track.toVector();
        for (size_t i = 0; i < tv.size(); ++i) {
            if (i) os << ",";
            os << tv[i].color;
        }
        os << "],";

        os << "\"pending\":[";
        auto pv = pending.toVector();
        for (size_t i = 0; i < pv.size(); ++i) {
            if (i) os << ",";
            os << pv[i].color;
        }
        os << "]";
        os << "}";
        return os.str();
    }

private:
    struct Snapshot {
        LinkedList<Ball> track;
        Queue<Ball> pending;
        int score = 0;
        int bombUses = 0;
        int recolorUses = 0;
        int skipUses = 0;
    };

    LinkedList<Ball> track;
    Queue<Ball> pending;
    Stack<Snapshot> history;
    GameStatus status = GameStatus::Playing;
    std::mt19937 rng;

    int randomColor() {
        std::uniform_int_distribution<int> d(1, numColors);
        return d(rng);
    }

    // 在 insert 时检测：把 c 放在 idx（之前）位置后，会不会立刻形成 3 连色。
    bool wouldCreateTriple(size_t idx, int c) {
        std::vector<int> v = collectColors();
        v.insert(v.begin() + std::min(idx, v.size()), c);
        // 找 idx 周围有没有 3 连。
        size_t L = idx, R = idx;
        while (L > 0 && v[L - 1] == c) --L;
        while (R + 1 < v.size() && v[R + 1] == c) ++R;
        return (R - L + 1) >= 3;
    }

    std::vector<int> collectColors() const {
        std::vector<int> out;
        auto vs = track.toVector();
        out.reserve(vs.size());
        for (auto& b : vs) out.push_back(b.color);
        return out;
    }

    // 以 idx 为起点扫描左右扩展段，若 >=3 同色则删除，并重复直到无可消除。
    void eliminateAround(size_t idx, int& totalEliminated, int& chainCount) {
        totalEliminated = 0;
        chainCount = 0;
        size_t pivot = idx;
        while (true) {
            if (track.empty()) break;
            if (pivot >= track.size()) pivot = track.size() - 1;

            auto* center = track.nodeAt(pivot);
            if (!center) break;
            int c = center->value.color;

            // 向左和向右各走，扩展连色范围
            auto* L = center;
            while (L->prev && L->prev != track.sentinelHead() && L->prev->value.color == c) L = L->prev;
            auto* R = center;
            while (R->next && R->next != track.sentinelTail() && R->next->value.color == c) R = R->next;

            int run = 0;
            for (auto* p = L; p != R->next; p = p->next) ++run;
            if (run < 3) break;

            // 记录左边邻居的下标，以便下一轮从相邻处继续
            int leftIdx = -1;
            {
                int i = 0;
                for (auto* p = track.sentinelHead()->next; p != track.sentinelTail(); p = p->next, ++i) {
                    if (p == L) { leftIdx = i - 1; break; }
                }
            }

            // 删除 [L, R]。先保存终止指针，避免 R 释放后读取 R->next。
            auto* stop = R->next;
            auto* p = L;
            while (p != stop) {
                auto* nx = p->next;
                track.remove(p);
                ++totalEliminated;
                p = nx;
            }
            ++chainCount;

            if (track.empty()) break;
            // 下一轮：从被删段左侧的球开始检查
            if (leftIdx < 0) pivot = 0;
            else if (static_cast<size_t>(leftIdx) >= track.size()) pivot = track.size() - 1;
            else pivot = static_cast<size_t>(leftIdx);
        }
    }

    void snapshot() {
        Snapshot s;
        s.track = track;
        s.pending = pending;
        s.score = score;
        s.bombUses = bombUses;
        s.recolorUses = recolorUses;
        s.skipUses = skipUses;
        history.push(s);
    }

    void applySnapshot(const Snapshot& s) {
        track = s.track;
        pending = s.pending;
        score = s.score;
        bombUses = s.bombUses;
        recolorUses = s.recolorUses;
        skipUses = s.skipUses;
    }

    void updateStatus() {
        if (track.empty() && pending.empty()) status = GameStatus::Won;
        else if (track.empty() && !pending.empty()) status = GameStatus::Won;  // 清空即胜利
        else if (track.size() >= maxTrackLength) status = GameStatus::Lost;
        else if (pending.empty()) {
            // 用尽待发射球，但轨道非空 -> 失败
            status = GameStatus::Lost;
        } else {
            status = GameStatus::Playing;
        }
    }

    const char* statusString() const {
        switch (status) {
            case GameStatus::Playing: return "playing";
            case GameStatus::Won: return "won";
            case GameStatus::Lost: return "lost";
        }
        return "playing";
    }
};
