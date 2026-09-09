#ifndef YATDS_SEARCH_BASELINE_B_TREE_B_PLUS_TREE_H_
#define YATDS_SEARCH_BASELINE_B_TREE_B_PLUS_TREE_H_

#include <memory>
#include <vector>

using namespace std;

namespace yatds::search::multiway_tree {

struct SearchResult {
  bool found = false;
  int comparisons = 0;
};

class BTree {
 public:
  explicit BTree(int minimum_degree = 2);

  bool Insert(int key);
  SearchResult Search(int key) const;
  vector<int> Traverse() const;

 private:
  struct Node {
    explicit Node(bool leaf_node) : leaf(leaf_node) {}

    bool leaf = true;
    vector<int> keys;
    vector<unique_ptr<Node>> children;
  };

  SearchResult Search(const Node* node, int key, int comparisons) const;
  void Traverse(const Node* node, vector<int>& output) const;
  void SplitChild(Node* parent, int child_index);
  void InsertNonFull(Node* node, int key);

  int t_;
  unique_ptr<Node> root_;
};

class BPlusTree {
 public:
  explicit BPlusTree(int order = 3);

  bool Insert(int key);
  SearchResult Search(int key) const;
  vector<int> TraverseLeaves() const;

 private:
  struct Node {
    explicit Node(bool leaf_node) : leaf(leaf_node) {}

    bool leaf = true;
    vector<int> keys;
    vector<unique_ptr<Node>> children;
    Node* next = nullptr;
  };

  struct InsertResult {
    bool inserted = false;
    bool split = false;
    int promoted_key = 0;
    unique_ptr<Node> new_child;
  };

  SearchResult Search(const Node* node, int key, int comparisons) const;
  InsertResult Insert(Node* node, int key);
  int FirstKey(const Node* node) const;

  int order_;
  unique_ptr<Node> root_;
};

}  // namespace yatds::search::multiway_tree

#endif  // YATDS_SEARCH_BASELINE_B_TREE_B_PLUS_TREE_H_
