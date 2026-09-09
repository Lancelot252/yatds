#ifndef YATDS_SEARCH_BASELINE_TRIE_H_
#define YATDS_SEARCH_BASELINE_TRIE_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace yatds::search::trie {

class Trie {
 public:
  void Insert(const string& word);
  bool Search(const string& word) const;
  bool StartsWith(const string& prefix) const;
  bool Remove(const string& word);

 private:
  struct Node {
    bool is_word = false;
    vector<pair<char, unique_ptr<Node>>> children;
  };

  Node* FindChild(Node* node, char ch) const;
  const Node* FindChild(const Node* node, char ch) const;
  bool Remove(Node* node, const string& word, size_t depth);

  Node root_;
};

}  // namespace yatds::search::trie

#endif  // YATDS_SEARCH_BASELINE_TRIE_H_
