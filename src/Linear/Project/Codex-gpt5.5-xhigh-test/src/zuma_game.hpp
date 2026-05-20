#ifndef ZUMA_GAME_HPP
#define ZUMA_GAME_HPP

#include <cstddef>
#include <list>
#include <queue>
#include <stack>
#include <string>
#include <vector>

class ZumaGame {
public:
    enum class Status {
        Playing,
        Won,
        Lost
    };

    ZumaGame();

    void reset();
    bool insertBall(std::size_t position);
    bool undo();
    bool usePowerup(const std::string& type, std::size_t position);
    bool rotateLauncher();

    std::string stateJson() const;

private:
    struct Snapshot {
        std::vector<char> track;
        std::vector<char> launcher;
        int score = 0;
        int moves = 0;
        int bombs = 0;
        int sweeps = 0;
        int rotates = 0;
        Status status = Status::Playing;
    };

    static constexpr std::size_t kMaxTrackSize = 28;

    std::list<char> track_;
    std::queue<char> launcher_;
    std::stack<Snapshot> undoStack_;
    int score_ = 0;
    int moves_ = 0;
    int bombs_ = 0;
    int sweeps_ = 0;
    int rotates_ = 0;
    int lastCombo_ = 0;
    Status status_ = Status::Playing;
    std::string message_;

    std::list<char>::iterator iteratorAt(std::size_t position);
    std::list<char>::const_iterator constIteratorAt(std::size_t position) const;
    Snapshot makeSnapshot() const;
    void restoreSnapshot(const Snapshot& snapshot);
    void pushUndo();
    int resolveEliminations();
    void checkGameOver();
    std::vector<char> launcherVector() const;
    std::string statusText() const;
};

#endif
