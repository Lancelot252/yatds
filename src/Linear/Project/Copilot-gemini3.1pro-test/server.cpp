#include "httplib.h"
#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <sstream>

using namespace std;
using namespace httplib;

struct GameState {
    list<char> track;
    vector<char> pending;
    int score;
};

class ZumaGame {
private:
    list<char> track;
    vector<char> pending; // Use vector to act as queue for easier serialization
    stack<GameState> history;
    int score;
    bool isWon;
    bool isLost;
    const int maxTrackLength = 30;

    void saveState() {
        history.push({ track, pending, score });
    }

    void checkElimination(list<char>::iterator cur) {
        if (track.empty() || cur == track.end()) return;
        
        char clr = *cur;
        if (clr == 'X') return; // X is bomb, handles separately

        auto left = cur;
        auto right = cur;
        int count = 1;

        while (left != track.begin()) {
            auto l = left; --l;
            if (*l == clr) { count++; left = l; } else break;
        }

        while (true) {
            auto r = right; ++r;
            if (r != track.end() && *r == clr) { count++; right = r; } else break;
        }

        if (count >= 3) {
            score += count * 10;
            auto eraseEnd = right; ++eraseEnd;
            
            auto nextCheck = track.erase(left, eraseEnd); // erase returns iterator following the last removed
            
            if (track.empty()) {
                isWon = true;
                return;
            }

            // check chain elimination at the merging point
            if (nextCheck != track.begin() && nextCheck != track.end()) {
                auto prevCheck = nextCheck; --prevCheck;
                if (*prevCheck == *nextCheck) {
                    checkElimination(prevCheck); 
                }
            }
        }
    }

    void generatePending() {
        const char colors[] = {'R', 'G', 'B', 'Y'};
        while (pending.size() < 5) {
            // 5% chance of bomb
            if (rand() % 20 == 0) pending.push_back('X'); 
            else pending.push_back(colors[rand() % 4]);
        }
    }

public:
    ZumaGame() {
        srand(time(nullptr));
        reset();
    }

    void reset() {
        track.clear();
        pending.clear();
        while(!history.empty()) history.pop();
        score = 0;
        isWon = false;
        isLost = false;

        const char colors[] = {'R', 'G', 'B', 'Y'};
        for (int i=0; i<10; i++) {
            track.push_back(colors[rand() % 4]);
        }
        generatePending();
    }

    bool insertBall(int pos) {
        if (isWon || isLost || pos < 0 || pos > track.size()) return false;
        if (pending.empty()) return false;

        saveState();

        char ball = pending.front();
        pending.erase(pending.begin());
        generatePending();

        auto it = track.begin();
        for (int i=0; i<pos; i++) it++;

        if (ball == 'X') { // Bomb prop
            // remove surrounding 1 from left and right
            auto t = it; 
            if (t != track.begin()) { --t; it = track.erase(t); }
            if (it != track.end()) { it = track.erase(it); }
            score += 20;
        } else {
            it = track.insert(it, ball);
            checkElimination(it);
        }

        if (track.empty()) {
            isWon = true;
        } else if (track.size() >= maxTrackLength) {
            isLost = true;
        }
        return true;
    }

    bool undo() {
        if (history.empty()) return false;
        track = history.top().track;
        pending = history.top().pending;
        score = history.top().score;
        history.pop();
        isWon = false;
        isLost = false;
        return true;
    }

    string toJSON() {
        ostringstream ss;
        ss << "{";
        
        ss << "\"track\": [";
        bool first = true;
        for (char c : track) {
            if (!first) ss << ",";
            ss << "\"" << c << "\"";
            first = false;
        }
        ss << "],";

        ss << "\"pending\": [";
        first = true;
        for (char c : pending) {
            if (!first) ss << ",";
            ss << "\"" << c << "\"";
            first = false;
        }
        ss << "],";

        ss << "\"score\": " << score << ",";
        ss << "\"isWon\": " << (isWon ? "true" : "false") << ",";
        ss << "\"isLost\": " << (isLost ? "true" : "false") << ",";
        ss << "\"canUndo\": " << (history.empty() ? "false" : "true");
        ss << "}";
        return ss.str();
    }
};

string loadHTML() {
    FILE* f = fopen("index.html", "r");
    if (!f) return "<html><body>Create index.html in the same directory!</body></html>";
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    string content(size, 0);
    fread(&content[0], 1, size, f);
    fclose(f);
    return content;
}

int main() {
    Server svr;
    ZumaGame game;

    svr.Get("/", [&](const Request& req, Response& res) {
        res.set_content(loadHTML(), "text/html");
    });

    svr.Get("/api/state", [&](const Request& req, Response& res) {
        res.set_content(game.toJSON(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    svr.Post("/api/insert", [&](const Request& req, Response& res) {
        if (req.has_param("pos")) {
            int pos = stoi(req.get_param_value("pos"));
            game.insertBall(pos);
        }
        res.set_content(game.toJSON(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    svr.Post("/api/undo", [&](const Request& req, Response& res) {
        game.undo();
        res.set_content(game.toJSON(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });
    
    svr.Post("/api/reset", [&](const Request& req, Response& res) {
        game.reset();
        res.set_content(game.toJSON(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    cout << "Zuma server running on http://localhost:8080..." << endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}
