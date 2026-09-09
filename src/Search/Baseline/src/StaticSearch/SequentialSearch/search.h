#ifndef YATDS_SEARCH_BASELINE_SEQUENTIAL_SEARCH_H_
#define YATDS_SEARCH_BASELINE_SEQUENTIAL_SEARCH_H_

#include <cstddef>
#include <vector>

using namespace std;

namespace yatds::search::sequential {

struct SearchResult {
  bool found = false;
  size_t index = 0;
  int comparisons = 0;
};

SearchResult SequentialSearch(const vector<int>& data, int key);
SearchResult SentinelSequentialSearch(const vector<int>& data, int key);

}  // namespace yatds::search::sequential

#endif  // YATDS_SEARCH_BASELINE_SEQUENTIAL_SEARCH_H_
