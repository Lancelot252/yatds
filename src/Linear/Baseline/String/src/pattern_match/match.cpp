#include "match.hpp"

/* START: pattern_match implementation */

int dsaac::naiveMatch(const std::string& text, const std::string& pattern)
{
    int n = static_cast<int>(text.length());
    int m = static_cast<int>(pattern.length());
    
    if (m == 0) return 0;

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && text[i + j] == pattern[j]) {
            j++;
        }
        if (j == m) {
            return i; // 匹配成功，返回起点
        }
    }
    return -1; // 匹配失败
}

std::vector<int> dsaac::getNext(const std::string& pattern)
{
    int m = static_cast<int>(pattern.length());
    std::vector<int> next(m, 0);
    
    if (m == 0) return next;

    int head = 0;
    for (int tail = 1; tail < m; tail++) {
        while (head > 0 && pattern[head] != pattern[tail]) {
            head = next[head - 1];
        }
        if (pattern[head] == pattern[tail]) {
            head++;
        }
        next[tail] = head;
    }
    return next;
}

int dsaac::kmpMatch(const std::string& text, const std::string& pattern)
{
    // 学生自行实现完整的 KMP 匹配流程...
    return -1;
}

/* END */