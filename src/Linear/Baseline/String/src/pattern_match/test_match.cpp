#include "match.hpp"
#include <iostream>
#include <string>
#include <chrono> // 引入计时库用于性能测试

int main()
{
    // ==========================================
    // Step 1: 分析与测试朴素算法
    // ==========================================
    std::cout << "--- Step 1: Naive Match ---" << "\n";
    // 提示：你可以构造极端的测试用例。例如主串为包含 1000 万个 'a' 开头加一个 'b'，模式串为 1000 个 'a' 后加一个 'b'。
    std::string test_text = "ababcabcacbab";
    std::string test_pattern = "abcac";
    
    int naiveRes = naiveMatch(test_text, test_pattern);
    std::cout << "Naive Match found at index: " << naiveRes << "\n\n";
    
    // ==========================================
    // Step 2: 基础 KMP 算法的实现
    // ==========================================
    std::cout << "--- Step 2: KMP Match ---" << "\n";
    std::vector<int> nxt = getNext(test_pattern);
    std::cout << "Next array of pattern: ";
    for (int val : nxt) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    
    // int kmpRes = kmpMatch(test_text, test_pattern);
    // std::cout << "KMP Match found at index: " << kmpRes << "\n\n";

    // ==========================================
    // Step 3: nextval 数组深度优化
    // ==========================================
    std::cout << "--- Step 3: nextval Optimization ---" << "\n";
    // 提示：使用具有多重复片段的测试样例，打印 nextval 并与 next 进行对比
    // std::vector<int> nxtVal = getNextVal(test_pattern);
    // std::cout << "NextVal array of pattern: ";
    // for (int val : nxtVal) {
    //     std::cout << val << " ";
    // }
    // std::cout << "\n";

    return 0;
}