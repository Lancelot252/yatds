#include "zuma_game.hpp"
#include <iostream>
#include <sstream>

void printTrack(const std::vector<char>& track) {
    std::cout << "轨道: ";
    for (char c : track) std::cout << c << ' ';
    std::cout << std::endl;
}
void printQueue(const std::vector<char>& queue) {
    std::cout << "待发射: ";
    for (char c : queue) std::cout << c << ' ';
    std::cout << std::endl;
}

int main() {
    ZumaGame game({'R','R','B','B','B','R','R'}, {'G','Y','B'});
    std::string cmd;
    while (true) {
        printTrack(game.getTrack());
        printQueue(game.getQueue());
        if (game.isWin()) { std::cout << "你赢了！\n"; break; }
        if (game.isLose()) { std::cout << "你输了！\n"; break; }
        std::cout << "输入插入位置(0~n)或u撤销或q退出: ";
        std::getline(std::cin, cmd);
        if (cmd == "q") break;
        if (cmd == "u") {
            if (!game.undo()) std::cout << "无法撤销\n";
            continue;
        }
        std::istringstream iss(cmd);
        int pos;
        if (iss >> pos) {
            std::cout << game.fire(pos) << std::endl;
        } else {
            std::cout << "无效输入\n";
        }
    }
    return 0;
}
