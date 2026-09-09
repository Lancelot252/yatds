#ifndef YATDS_SEARCH_BASELINE_BST_AVL_H_
#define YATDS_SEARCH_BASELINE_BST_AVL_H_

#include <memory>
#include <vector>

using namespace std;

namespace yatds::search::tree {

struct SearchResult {
  bool found = false;
  int comparisons = 0;
};

class BinarySearchTree {
 public:
  bool Insert(int key);
  bool Remove(int key);
  SearchResult Search(int key) const;
  vector<int> InOrder() const;

 private:
  struct Node {
    explicit Node(int value) : key(value) {}

    int key;
    unique_ptr<Node> left;
    unique_ptr<Node> right;
  };

  bool Remove(unique_ptr<Node>& node, int key);
  void InOrder(const unique_ptr<Node>& node, vector<int>& output) const;

  unique_ptr<Node> root_;
};

class AVLTree {
 public:
  bool Insert(int key);
  bool Remove(int key);
  SearchResult Search(int key) const;
  vector<int> InOrder() const;

 private:
  struct Node {
    explicit Node(int value) : key(value) {}

    int key;
    int height = 1;
    unique_ptr<Node> left;
    unique_ptr<Node> right;
  };

  int Height(const unique_ptr<Node>& node) const;
  void UpdateHeight(Node* node);
  int BalanceFactor(const unique_ptr<Node>& node) const;
  unique_ptr<Node> RotateLeft(unique_ptr<Node> node);
  unique_ptr<Node> RotateRight(unique_ptr<Node> node);
  unique_ptr<Node> Balance(unique_ptr<Node> node);
  unique_ptr<Node> Insert(unique_ptr<Node> node, int key, bool& inserted);
  unique_ptr<Node> Remove(unique_ptr<Node> node, int key, bool& removed);
  void InOrder(const unique_ptr<Node>& node, vector<int>& output) const;

  unique_ptr<Node> root_;
};

}  // namespace yatds::search::tree

#endif  // YATDS_SEARCH_BASELINE_BST_AVL_H_
