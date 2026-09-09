#ifndef YATDS_SEARCH_BASELINE_STATIC_TREE_TABLE_H_
#define YATDS_SEARCH_BASELINE_STATIC_TREE_TABLE_H_

#include <cstddef>
#include <unordered_set>
#include <vector>

using namespace std;

namespace yatds::search::static_tree_table {

struct SearchResult {
  bool found = false;
  size_t node_index = 0;
  int comparisons = 0;
};

class StaticTreeTable {
 public:
  explicit StaticTreeTable(const vector<int>& ordered_keys);

  SearchResult Search(int key) const;
  const vector<int>& Nodes() const { return nodes_; }

 private:
  void Build(const vector<int>& ordered_keys, int left, int right,
             size_t node_index);

  vector<int> nodes_;
  unordered_set<size_t> used_;
};

}  // namespace yatds::search::static_tree_table

#endif  // YATDS_SEARCH_BASELINE_STATIC_TREE_TABLE_H_
