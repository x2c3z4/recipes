#include "thirdparty/gtest/gtest.h"

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
  int maxDepth(TreeNode *root) {
    if(root == NULL) {
      return 0;
    }

    int left = maxDepth(root->left);
    int right = maxDepth(root->right);
    int rest = left > right? left : right;
    return rest + 1;
  }
};

TEST(BinTreePostorder, InitializeTree) {
  TreeNode a1(1), a2(2), a3(3), a4(4);
  TreeNode b1(11), b2(22), b3(33);
  TreeNode root(0);
  root.left = &a1;
  root.right = &b2;

  a1.left = &a2;
  a2.left = &a3;
  a3.left = &a4;

  b1.left = &b2;
  b2.left = &b3;

  Solution s;
  int result = s.maxDepth(&root);
  EXPECT_EQ(result, 5);
}
