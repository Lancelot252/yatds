#ifndef YATDS_SEARCH_BASELINE_ORDERED_SEARCH_H_
#define YATDS_SEARCH_BASELINE_ORDERED_SEARCH_H_

#include <cstddef>
#include <vector>

using namespace std;

namespace yatds::search::ordered {

struct SearchResult {
  bool found = false;
  size_t index = 0;
  int comparisons = 0;
};

SearchResult BinarySearch(const vector<int>& data, int key);
SearchResult InterpolationSearch(const vector<int>& data, int key);
SearchResult FibonacciSearch(const vector<int>& data, int key);

}  // namespace yatds::search::ordered

#endif  // YATDS_SEARCH_BASELINE_ORDERED_SEARCH_H_
