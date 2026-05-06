#ifndef DSAAC_PATTERN_MATCH_HPP
#define DSAAC_PATTERN_MATCH_HPP

#include <string>
#include <vector>

namespace dsaac {

int naiveMatch(const std::string& text, const std::string& pattern);
std::vector<int> getNext(const std::string& pattern);
int kmpMatch(const std::string& text, const std::string& pattern);

// Step 3 进阶优化要求：
std::vector<int> getNextVal(const std::string& pattern);

} // namespace dsaac

// 为了方便使用，将方法暴露在全局（也可按需不暴露）
using dsaac::naiveMatch;
using dsaac::getNext;
using dsaac::kmpMatch;
using dsaac::getNextVal;

#endif // DSAAC_PATTERN_MATCH_HPP