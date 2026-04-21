#include "list.hpp"
#include <iostream>

int main()
{
    LinkedList<int> list;

    for (int i = 0; i < 10; ++i) {
        list.push_back(i);
    }
    list.erase(4);

    for (int value : list.to_vector()) {
        std::cout << value << ' ';
    }
    std::cout << "\ncontains(4): " << std::boolalpha << list.contains(4) << '\n';
    return 0;
}
