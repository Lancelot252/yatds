#pragma once
#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// 利用链表、队列、栈
class ZumaGame {
public:
    struct Snapshot {
        std::list<char> board;
        std::queue<char> next_balls;
    };

    std::list<char> board;
    std::queue<char> next_balls;
    std::stack<Snapshot> undo_stack;

    const std::vector<char> colors = {'R', 'B', 'G', 'Y'};

    ZumaGame() {
        std::srand(std::time(nullptr));
        init_game();
    }

    void init_game() {
        board.clear();
        while(!next_balls.empty()) next_balls.pop();
        while(!undo_stack.empty()) undo_stack.pop();

        for (int i = 0; i < 10; ++i) {
            board.push_back(colors[std::rand() % colors.size()]);
        }
        for (int i = 0; i < 5; ++i) {
            next_balls.push(colors[std::rand() % colors.size()]);
        }
    }

    void save_snapshot() {
        undo_stack.push({board, next_balls});
    }

    void undo() {
        if (!undo_stack.empty()) {
            Snapshot s = undo_stack.top();
            undo_stack.pop();
            board = s.board;
            next_balls = s.next_balls;
        }
    }

    bool check_and_eliminate(std::list<char>::iterator inserted_pos) {
        if (board.empty()) return false;
        
        auto left = inserted_pos;
        auto right = inserted_pos;
        char color = *inserted_pos;
        int count = 1;

        while (left != board.begin()) {
            auto prev = std::prev(left);
            if (*prev == color) {
                left = prev;
                count++;
            } else break;
        }

        while (std::next(right) != board.end()) {
            auto next = std::next(right);
            if (*next == color) {
                right = next;
                count++;
            } else break;
        }

        if (count >= 3) {
            auto next_iter = std::next(right);
            board.erase(left, next_iter);
            
            // 连锁消除检查可以从 next_iter（如果没到结尾）或它的前一个节点判断
            if (!board.empty() && next_iter != board.end() && next_iter != board.begin()) {
                auto prev_iter = std::prev(next_iter);
                if (*prev_iter == *next_iter) {
                    check_and_eliminate(next_iter);
                }
            }
            return true;
        }
        return false;
    }

    bool insert_ball(int index) {
        if (index < 0 || index > board.size() || next_balls.empty()) return false;

        save_snapshot();
        
        char ball = next_balls.front();
        next_balls.pop();
        next_balls.push(colors[std::rand() % colors.size()]); // 补充新的球

        auto it = board.begin();
        std::advance(it, index);
        auto inserted_it = board.insert(it, ball);

        check_and_eliminate(inserted_it);
        return true;
    }

    std::string get_board_str() {
        std::string s;
        for (char c : board) s += c;
        return s;
    }

    std::string get_next_balls_str() {
        std::string s;
        std::queue<char> temp = next_balls;
        while (!temp.empty()) {
            s += temp.front();
            temp.pop();
        }
        return s;
    }
};
