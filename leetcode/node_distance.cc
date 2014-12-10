#include "thirdparty/gtest/gtest.h"

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
};

using namespace std;

class Solution {
public:
    int GetDistance(TreeNode *root, TreeNode *node1, TreeNode *node2) {
      TreeNode *lca = findLCA(root, node1, node2);
      int dis_lca = findLevel(root, lca);
      int dis1 = findLevel(root, node1);
      int dis2 = findLevel(root, node2);
      return dis1 + dis2 - 2 * dis_lca;
  }

  TreeNode* findLCA(TreeNode* root, TreeNode *node1, TreeNode *node2) {
    if (root == nullptr) return nullptr;
    if (root == node1 || root == node2) {
      return root;
    }
    TreeNode* left = findLCA(root->left, node1, node2);
    TreeNode* right = findLCA(root->right, node1, node2);
    if (left && right) {
      return root;
    }
    return left != nullptr? left : right;
  }

  int findLevel(TreeNode *root, TreeNode *node) {
    if (root == nullptr) {return -1;}
    if (root == node) {
      return 0;
    }
    int level = findLevel(root->left, node);
    if (level == -1) {
      level = findLevel(root->right, node);
    }

    if (level != -1) {
      level++;
    }
    return level;
  }
};

TEST(ThreeNode, count) {
  Solution s;
  TreeNode node1, node2, node3, node4, node5;
  node1.left = &node2;
  node2.left = &node3;
  node2.right = &node4;
  node1.right = &node5;

  TreeNode *root = &node1;
  cout << s.GetDistance(root, &node3, &node5) << endl;
}
