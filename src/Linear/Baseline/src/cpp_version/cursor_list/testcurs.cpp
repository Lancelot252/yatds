#include "cursor.hpp"
#include <iostream>

int main()
{
    CursorList<int, 100> list;
    for (int i = 0; i < 10; ++i) {
        list.push_back(i);
    }
    for (int i = 0; i < 10; i += 2) {
        list.erase(i);
    }

    for (int value : list.to_vector()) {
        std::cout << value << ' ';
    }
    std::cout << '\n';
    return 0;
}
