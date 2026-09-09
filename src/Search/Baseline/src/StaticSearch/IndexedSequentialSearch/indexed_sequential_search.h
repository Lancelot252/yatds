#ifndef YATDS_SEARCH_BASELINE_INDEXED_SEQUENTIAL_SEARCH_H_
#define YATDS_SEARCH_BASELINE_INDEXED_SEQUENTIAL_SEARCH_H_

#include <cstddef>
#include <vector>

using namespace std;

namespace yatds::search::indexed {

struct SearchResult {
  bool found = false;
  size_t index = 0;
  int comparisons = 0;
};

class IndexedSequentialTable {
 public:
  IndexedSequentialTable(const vector<int>& data, size_t block_size);

  SearchResult Search(int key) const;

 private:
  struct IndexEntry {
    int max_key = 0;
    size_t start = 0;
    size_t length = 0;
  };

  vector<int> data_;
  vector<IndexEntry> index_;
};

}  // namespace yatds::search::indexed

#endif  // YATDS_SEARCH_BASELINE_INDEXED_SEQUENTIAL_SEARCH_H_
