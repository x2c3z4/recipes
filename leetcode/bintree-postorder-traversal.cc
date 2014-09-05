#include <stdio.h>

#include <sstream>
#include <stack>
#include <algorithm>
#include "thirdparty/gtest/gtest.h"

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

void print_vector(vector<int>& vec) {
  std::ostringstream oss;
  for(int c: vec) {
    oss << c << ',';
  }
  printf("%s\n", oss.str().substr(0, oss.str().length() - 1).c_str());
}
class Solution {
public:
  bool is_leaf(TreeNode *node) {
    return (node->left==NULL && node->right==NULL);
  }
    void __postorderTraversal(TreeNode *root, vector<int> &vec) {
      if(root==NULL)
        return;
      stack<TreeNode*> stack_;
      stack_.push(root);
      // root right left, using preorder traversal
      // then reverse
      while(!stack_.empty()) {
        root = stack_.top();
        stack_.pop();
        vec.push_back(root->val);
        if(root->left) {
          stack_.push(root->left);
        }
        if(root->right) {
          stack_.push(root->right);
        }
    }
  }
    vector<int> postorderTraversal(TreeNode *root) {
      vector<int> vec;
      // __postorderTraversalRecur(root, vec);
      __postorderTraversal(root, vec);
      reverse(vec.begin(),vec.end());
      return vec;
    }
};

TEST(BinTreePostorder, InitializeTree) {
  TreeNode node1(1);
  TreeNode node2(2);
  // TreeNode node3(3);
  node1.left = &node2;
  // node2.left = &node3;

  Solution s;
  auto result = s.postorderTraversal(&node1);
  print_vector(result);
}
