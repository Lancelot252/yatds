#include "mystring.hpp"
#include <iostream>

int main()
{
    MyString s1("Hello");
    MyString s2(" World");
    
    std::cout << "s1 length: " << s1.length() << '\n';
    
    // s1.append(s2);
    // std::cout << "After append, s1 length: " << s1.length() << '\n';
    
    MyString sub = s1.substr(0, 4);
    std::cout << "Substr length: " << sub.length() << '\n';

    return 0;
}