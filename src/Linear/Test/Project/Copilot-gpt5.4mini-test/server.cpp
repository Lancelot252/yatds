#include "httplib.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <ctime>
#include <deque>
#include <fstream>
#include <iterator>
#include <list>
#include <random>
#include <sstream>
#include <string>

using namespace httplib;

struct GameSnapshot {
    std::list<char> track;
    std::deque<char> supply;
    int score;
    bool won;
    bool lost;
};

class ZumaGame {
public:
    ZumaGame() : rng_(static_cast<unsigned>(std::time(nullptr))) {
        reset();
    }

    void reset() {
        track_.clear();
        supply_.clear();
        while (!history_.empty()) {
            history_.pop();
        }

        score_ = 0;
        won_ = false;
        lost_ = false;

        const char colors[] = {'R', 'G', 'B', 'Y'};
        std::uniform_int_distribution<int> colorPick(0, 3);
        for (int i = 0; i < 10; ++i) {
            track_.push_back(colors[colorPick(rng_)]);
        }

        refillSupply();
    }

    bool insertBall(std::size_t position) {
        if (won_ || lost_ || supply_.empty()) {
            return false;
        }
        if (position > track_.size()) {
            return false;
        }

        saveSnapshot();

        char ball = supply_.front();
        supply_.pop_front();
        refillSupply();

        auto insertPoint = track_.begin();
        std::advance(insertPoint, static_cast<long>(position));

        if (ball == 'X') {
            applyBomb(insertPoint);
        } else {
            auto inserted = track_.insert(insertPoint, ball);
            resolveChains(inserted);
        }

        if (track_.empty()) {
            won_ = true;
        } else if (track_.size() >= maxTrackLength_) {
            lost_ = true;
        }

        return true;
    }

    bool undo() {
        if (history_.empty()) {
            return false;
        }

        const auto &snapshot = history_.top();
        track_ = snapshot.track;
        supply_ = snapshot.supply;
        score_ = snapshot.score;
        won_ = snapshot.won;
        lost_ = snapshot.lost;
        history_.pop();
        return true;
    }

    std::string toJson() const {
        std::ostringstream out;
        out << '{';
        out << "\"track\":" << stringifyTrack(track_) << ',';
        out << "\"supply\":" << stringifyTrack(supply_) << ',';
        out << "\"score\":" << score_ << ',';
        out << "\"won\":" << (won_ ? "true" : "false") << ',';
        out << "\"lost\":" << (lost_ ? "true" : "false") << ',';
        out << "\"canUndo\":" << (history_.empty() ? "false" : "true") << ',';
        out << "\"trackLimit\":" << maxTrackLength_;
        out << '}';
        return out.str();
    }

private:
    static constexpr std::size_t supplySize_ = 5;
    static constexpr std::size_t maxTrackLength_ = 40;

    std::list<char> track_;
    std::deque<char> supply_;
    std::stack<GameSnapshot> history_;
    int score_ = 0;
    bool won_ = false;
    bool lost_ = false;
    std::mt19937 rng_;

    static std::string stringifyTrack(const std::list<char> &items) {
        std::ostringstream out;
        out << '[';
        bool first = true;
        for (char item : items) {
            if (!first) {
                out << ',';
            }
            out << '"' << item << '"';
            first = false;
        }
        out << ']';
        return out.str();
    }

    static std::string stringifyTrack(const std::deque<char> &items) {
        std::ostringstream out;
        out << '[';
        bool first = true;
        for (char item : items) {
            if (!first) {
                out << ',';
            }
            out << '"' << item << '"';
            first = false;
        }
        out << ']';
        return out.str();
    }

    void saveSnapshot() {
        history_.push(GameSnapshot{track_, supply_, score_, won_, lost_});
    }

    void refillSupply() {
        const char colors[] = {'R', 'G', 'B', 'Y'};
        std::uniform_int_distribution<int> colorPick(0, 3);
        std::uniform_int_distribution<int> bombPick(0, 24);

        while (supply_.size() < supplySize_) {
            if (bombPick(rng_) == 0) {
                supply_.push_back('X');
            } else {
                supply_.push_back(colors[colorPick(rng_)]);
            }
        }
    }

    void applyBomb(std::list<char>::iterator insertPoint) {
        if (insertPoint != track_.begin()) {
            auto left = insertPoint;
            --left;
            insertPoint = track_.erase(left);
        }

        if (insertPoint != track_.end()) {
            insertPoint = track_.erase(insertPoint);
        }

        score_ += 20;

        if (!track_.empty()) {
            auto probe = insertPoint;
            if (probe == track_.end()) {
                probe = std::prev(track_.end());
            }
            resolveChains(probe);
        }
    }

    void resolveChains(std::list<char>::iterator pivot) {
        if (track_.empty() || pivot == track_.end()) {
            return;
        }

        const char color = *pivot;
        if (color == 'X') {
            return;
        }

        auto left = pivot;
        while (left != track_.begin()) {
            auto previous = left;
            --previous;
            if (*previous != color) {
                break;
            }
            left = previous;
        }

        auto right = pivot;
        while (true) {
            auto next = right;
            ++next;
            if (next == track_.end() || *next != color) {
                break;
            }
            right = next;
        }

        const auto count = static_cast<std::size_t>(std::distance(left, std::next(right)));
        if (count < 3) {
            return;
        }

        score_ += static_cast<int>(count) * 10;
        auto after = track_.erase(left, std::next(right));
        if (track_.empty()) {
            won_ = true;
            return;
        }

        if (after == track_.end()) {
            after = std::prev(track_.end());
        }

        resolveChains(after);
    }
};

static std::string readFile(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return R"(<html><body><h1>index.html not found</h1></body></html>)";
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    Server server;
    ZumaGame game;

    server.Get("/", [&](const Request &, Response &res) {
        res.set_content(readFile("index.html"), "text/html; charset=utf-8");
    });

    server.Get("/api/state", [&](const Request &, Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(game.toJson(), "application/json; charset=utf-8");
    });

    server.Post("/api/insert", [&](const Request &req, Response &res) {
        if (req.has_param("pos")) {
            const auto posText = req.get_param_value("pos");
            try {
                game.insertBall(static_cast<std::size_t>(std::stoul(posText)));
            } catch (...) {
            }
        }
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(game.toJson(), "application/json; charset=utf-8");
    });

    server.Post("/api/undo", [&](const Request &, Response &res) {
        game.undo();
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(game.toJson(), "application/json; charset=utf-8");
    });

    server.Post("/api/reset", [&](const Request &, Response &res) {
        game.reset();
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(game.toJson(), "application/json; charset=utf-8");
    });

    server.set_mount_point("/", ".");
    server.listen("0.0.0.0", 8080);
    return 0;
}