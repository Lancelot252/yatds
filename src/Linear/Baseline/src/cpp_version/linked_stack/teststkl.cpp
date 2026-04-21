#include "stackli.hpp"
#include <iostream>

int main()
{
    LinkedStack<int> stack;
    for (int i = 0; i < 10; ++i) {
        stack.push(i);
    }
    while (!stack.empty()) {
        std::cout << stack.top() << '\n';
        stack.pop();
    }
    return 0;
}
