#include <stdio.h>

#include <sstream>
#include <stack>
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
    void __preorderTraversal(TreeNode *root, vector<int> &vec) {
      stack<TreeNode*> stack_;
      while(root != NULL) {
        vec.push_back(root->val);
        if(root->left) {
          if(root->right) {
            stack_.push(root->right);
          }
          root = root->left;
        } else {
          if(root->right) {
            root = root->right;
          } else {
            if(stack_.empty()){
              break;
            }
            root = stack_.top();
            stack_.pop();
          }
        }
      }
    }

    // void __preorderTraversalRecur(TreeNode *root, vector<int> &vec) {
    //   if(root != NULL) {
    //     vec.push_back(root->val);
    //     __preorderTraversalRecur(root, vec);
    //     __preorderTraversalRecur(root, vec);
    //   }
    // }

    vector<int> preorderTraversal(TreeNode *root) {
      vector<int> vec;
      // __preorderTraversalRecur(root, vec);
      __preorderTraversal(root, vec);
      return vec;
    }
};

TEST(BinTreePreOrder, InitializeTree) {
  TreeNode node1(1);
  TreeNode node2(2);
  // TreeNode node3(3);
  node1.left = &node2;
  // node2.left = &node3;

  Solution s;
  auto result = s.preorderTraversal(&node1);
  print_vector(result);
}
