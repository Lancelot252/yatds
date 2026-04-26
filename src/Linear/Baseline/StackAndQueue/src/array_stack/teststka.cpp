#include "stackar.hpp"
#include <iostream>

int main()
{
    ArrayStack<int> stack(12);
    for (int i = 0; i < 10; ++i) {
        stack.push(i);
    }
    while (!stack.empty()) {
        std::cout << stack.top() << '\n';
        stack.pop();
    }
    return 0;
}
