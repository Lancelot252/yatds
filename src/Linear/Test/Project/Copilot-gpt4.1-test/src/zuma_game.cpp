#include "zuma_game.hpp"
#include <algorithm>

ZumaGame::ZumaGame(const std::vector<char>& initTrack, const std::vector<char>& initQueue)
    : track(initTrack), queue(initQueue) {}

std::string ZumaGame::fire(int pos) {
    if (queue.empty() || pos < 0 || pos > (int)track.size()) return "无效操作";
    history.push({track, queue});
    char ball = queue.front();
    queue.erase(queue.begin());
    track.insert(track.begin() + pos, ball);
    eliminate();
    return "发射并消除完成";
}

void ZumaGame::eliminate() {
    bool changed = true;
    while (changed) {
        changed = false;
        int n = track.size();
        for (int i = 0; i < n;) {
            int j = i + 1;
            while (j < n && track[j] == track[i]) ++j;
            if (j - i >= 3) {
                track.erase(track.begin() + i, track.begin() + j);
                changed = true;
                n = track.size();
                break;
            } else {
                i = j;
            }
        }
    }
}

bool ZumaGame::undo() {
    if (history.empty()) return false;
    track = history.top().first;
    queue = history.top().second;
    history.pop();
    return true;
}

std::vector<char> ZumaGame::getTrack() const { return track; }
std::vector<char> ZumaGame::getQueue() const { return queue; }
bool ZumaGame::isWin() const { return track.empty(); }
bool ZumaGame::isLose() const { return queue.empty() && !track.empty(); }
