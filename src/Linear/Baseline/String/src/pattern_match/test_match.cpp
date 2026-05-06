#include "match.hpp"
#include <iostream>

int main()
{
    std::string text = "ababcabcacbab";
    std::string pattern = "abcac";
    
    int naiveRes = naiveMatch(text, pattern);
    std::cout << "Naive Match found at index: " << naiveRes << "\n";
    
    std::vector<int> nxt = getNext(pattern);
    std::cout << "Next array of pattern: ";
    for (int val : nxt) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    
    // int kmpRes = kmpMatch(text, pattern);
    // std::cout << "KMP Match found at index: " << kmpRes << "\n";
    
    return 0;
}