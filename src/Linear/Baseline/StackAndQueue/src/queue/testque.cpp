#include "queue.hpp"
#include <iostream>

int main()
{
    CircularQueue<int> queue(12);
    for (int i = 0; i < 10; ++i) {
        queue.push(i);
    }
    while (!queue.empty()) {
        std::cout << queue.front() << '\n';
        queue.pop();
    }
    return 0;
}
